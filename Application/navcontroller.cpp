#include "navcontroller.h"
#include "Command.h"
#include "navsettingmodel.h"
#include "navsettingview.h"
#include "qtserialportimpl.h"
#include "serialport.h"
#include <QTimerEvent>
#include <atomic>
#include <cstdint>
#include <qdebug.h>
#include <qstringview.h>
#include <qtmetamacros.h>


NavController::NavController(QWidget* parent)
{
    view      = new NavSettingView(channelNum, parent);
    models[1] = new NavSettingModel(channelNum);
    models[2] = new NavSettingModel(channelNum);
    models[3] = new NavSettingModel(channelNum);
    enableListen.store(false, std::memory_order_relaxed);
}

void NavController::Initialize()
{
    view->Initialize();
    connect(view, &NavSettingView::Open, [this](const QString& name,int baud) {
        if (device) {
            StopListen();
            delete device;
        }
        device = new SerialPort(new QtSerialPortImpl(name, baud, 2, 8, 2));
        if (device->Open()) {
            qDebug() << "open serial port success,begin listen thread:";
            StartListen();
        }
        Read(currentIndex);
    });
    connect(view, &NavSettingView::Close, [this]() {
        if (device) {
            device->Close();
            StopListen();
        }
    });
    connect(view, &NavSettingView::StopListen, [this]() { this->StopListen(); });

    connect(view, &NavSettingView::RequestSignalSource, [this]() {
        if (device) {
            Request(Command::GenerateRequestSignalSourceCmd(currentIndex));
        }
    });

    connect(view, &NavSettingView::RequestReverse, [this]() {
        if (device) {
            Request(Command::GenerateRequestReverseCmd(currentIndex));
        }
    });
    connect(view, &NavSettingView::RequestMinimalHelm, [this]() {
        if (device) {
            Request(Command::GenerateRequestMinimalHelm(currentIndex));
        }
    });

    connect(view, &NavSettingView::RequestMaximalHelm, [this]() {
        if (device) {
            Request(Command::GenerateRequestMaximalHelm(currentIndex));
        }
    });

    connect(view, &NavSettingView::RequestFineTune, [this]() {
        if (device) {
            Request(Command::GenerateRequestFineTune(currentIndex));
            Request(Command::GenerateRequestMaximalHelm(currentIndex));
            Request(Command::GenerateRequestMinimalHelm(currentIndex));
            Request(Command::GenerateRequestReverseCmd(currentIndex));
            Request(Command::GenerateRequestSignalSourceCmd(currentIndex));
        }
    });

    connect(view, &NavSettingView::WriteSignalSource, [this]() {
        if (device) {
            device->Write(Command::GenerateMappSlotCmd(currentIndex, Model()));
        }
    });
    connect(view, &NavSettingView::WriteReverse, [this]() {
        if (device) {
            device->Write(Command::GenerateReverseCmd(currentIndex, Model()));
        }
    });
    connect(view, &NavSettingView::WriteMinimalHelm, [this]() {
        if (device) {
            device->Write(Command::GenerateMinimalHelmCmd(currentIndex, Model()));
        }
    });
    connect(view, &NavSettingView::WriteMaximalHelm, [this]() {
        if (device) {
            device->Write(Command::GenerateMaximalHelmCmd(currentIndex, Model()));
        }
    });
    connect(view, &NavSettingView::WriteMiddleHelm, [this]() {
        if (device) {
            device->Write(Command::GenerateMiddleHelmCmd(currentIndex, Model()));
        }
    });
    connect(view, &NavSettingView::MapperChanged, [this](int channel, int val) { Model()->SetMapSlot(channel, val); });
    connect(view, &NavSettingView::ReveseChanged, [this](int channel, bool val) { Model()->SetReverse(channel, val); });
    connect(view, &NavSettingView::MinimalHelmChanged, [this](int channel, int val) { Model()->SetMinimalHelm(channel, val); });
    connect(view, &NavSettingView::MiddleHeelmChanged, [this](int channel, int val) { Model()->SetMiddlelHelm(channel, val); });
    connect(view, &NavSettingView::MaximalHelmChanged, [this](int channel, int val) { Model()->SetMaximalHelm(channel, val); });
    connect(view, &NavSettingView::sbusChanged, [this](int index) {
        this->currentIndex = index + 1;
        if (device) {
            this->Read(currentIndex);
        }
    });

    connect(view, &NavSettingView::Read, [this]() {
        if (device) {
            this->Read(currentIndex);
        }
    });

    connect(view, &NavSettingView::Write, [this]() {
        if (device) {
            this->Write(currentIndex);
        }
    });
}

QWidget* NavController::View()
{
    return view;
}

void NavController::Request(const QByteArray& data)
{
    device->Write(data);
}

void NavController::StartListen()
{
    enableListen.store(true, std::memory_order_relaxed);
    writeTimerId = startTimer(writeInterval);
    readTimerId  = startTimer(readInterval);
}

void NavController::Read(uint8_t sbus)
{
    {
        std::lock_guard<std::mutex> lock(commandMtx);
        commands.push(Command::GenerateRequestFineTune(sbus));
        commands.push(Command::GenerateRequestMaximalHelm(sbus));
        commands.push(Command::GenerateRequestMinimalHelm(sbus));
        commands.push(Command::GenerateRequestReverseCmd(sbus));
        commands.push(Command::GenerateRequestSignalSourceCmd(sbus));
        //commands.push(Command::GenerateRequestPositionSourceCmd(sbus));
    }
}

void NavController::Write(uint8_t sbus)
{
    {
        std::lock_guard<std::mutex> lock(immediateMtx);
        immidiateCommands.push(Command::GenerateMappSlotCmd(sbus, Model()));
        immidiateCommands.push(Command::GenerateReverseCmd(sbus, Model()));
        immidiateCommands.push(Command::GenerateMinimalHelmCmd(sbus, Model()));
        immidiateCommands.push(Command::GenerateMaximalHelmCmd(sbus, Model()));
        immidiateCommands.push(Command::GenerateMiddleHelmCmd(sbus, Model()));
    }
}

void NavController::StopListen()
{
    if (enableListen.load()) {
        enableListen.store(false, std::memory_order_relaxed);
        killTimer(writeTimerId);
        killTimer(readTimerId);
    }
}

void NavController::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == writeTimerId) {

        if (!commands.empty()) {
            if (respondCommand == lastCommand) {
                {
                    std::lock_guard<std::mutex> lock(commandMtx);
                    commands.pop();
                }
            }
            if (!commands.empty()) {
                device->Write(commands.front());
                lastCommand = commands.front()[Command::cmdBit];
            }
        }
        if (!immidiateCommands.empty()) {
            device->Write(immidiateCommands.front());
            {
                std::lock_guard<std::mutex> lock(immediateMtx);
                immidiateCommands.pop();
            }
        }
    }
    if (event->timerId() == readTimerId) {
        if (enableListen.load(std::memory_order_relaxed)) {
            auto msg = device->Read();
            if (msg.size() >= 38) {
                msg.resize(38);
                if (Command::ParityRespond(msg)) {
                    ParseRespond(msg);
                    msg = msg.toHex(' ');
                    emit MessageChanged(msg);
                }
            }
            else if (msg.size() == 25) {
                if (ParseSbusFramebuffer(msg)) {
                    msg = msg.toHex(' ');
                    emit MessageChanged(msg);
                }
            }
        }
    }
}

void NavController::ParseRespond(const QByteArray& data)
{
    if (data.size() < 38) {
        return;
    }
    uint8_t cmdBit, ctrBit;
    cmdBit     = (unsigned char)data[Command::cmdBit];
    ctrBit     = (unsigned char)data[Command::ctrBit];
    auto model = models.find(ctrBit);
    if (model == models.end()) {
        return;
    }
    const unsigned char* ptr = (unsigned char*)(data.data() + Command::dataBit);

    auto parseUnsignedValue = [&ptr]() -> unsigned short {
        unsigned short val = 0x0000;
        unsigned char low  = *ptr++;
        unsigned char high = *ptr++;
        val |= high;
        val = val << 8;
        val |= low;
        return val;
    };

    auto parseValue = [&ptr]() -> short {
        short val         = 0x0000;
        unsigned char low = *ptr++;
        char high         = *(char*)ptr++;
        val |= high;
        val = val << 8;
        val |= low;
        return val;
    };

    switch (cmdBit) {
    case Command::signalSource:
        for (int i = 0; i < channelNum; ++i) {
            auto val = parseUnsignedValue();
            model->second->SetMapSlot(i, val);
        }
        break;
    case Command::reverse:
        for (int i = 0; i < channelNum; ++i) {
            auto val = parseUnsignedValue();
            model->second->SetReverse(i, val);
        }
        break;
    case Command::minimalHelm:
        for (int i = 0; i < channelNum; ++i) {
            auto val = parseUnsignedValue();
            model->second->SetMinimalHelm(i, val);
        }
        break;
    case Command::maximalHelm:
        for (int i = 0; i < channelNum; ++i) {
            auto val = parseUnsignedValue();
            model->second->SetMaximalHelm(i, val);
        }
        break;
    case Command::fineTune:
        for (int i = 0; i < channelNum; ++i) {
            auto val = parseValue();
            model->second->SetMiddlelHelm(i, val);
        }
        break;
    case Command::position:
        for (int i = 0; i < channelNum; ++i) {
            auto val = parseUnsignedValue();
            model->second->SetPosition(i, val);
        }
        break;
    }
    view->SetModel(model->second, currentIndex);
    respondCommand.store(cmdBit);
}

bool NavController::ParseSbusFramebuffer(const QByteArray& data)
{

    if (data.size() != 25) {}
    if (data[0] != 0x0f || data[24] != 0x00) {
        return false;
    }
    uint8_t* ptr = (uint8_t*)data.data();
    ++ptr;

    auto model = Model();
    if (!model) {
        return false;
    }
	uint16_t result[16];
	result[0] = ((data[2] << 8) + (data[1])) & 0x07ff;
	result[1] = ((data[3] << 5) + (data[2] >> 3)) & 0x07ff;
	result[2] = ((data[5] << 10) + (data[4] << 2) + (data[3] >> 6)) & 0x07ff;
	result[3] = ((data[6] << 7) + (data[5] >> 1)) & 0x07ff;
	result[4] = ((data[7] << 4) + (data[6] >> 4)) & 0x07ff;
	result[5] = ((data[9] << 9) + (data[8] << 1) + (data[7] >> 7)) & 0x07ff;
	result[6] = ((data[10] << 6) + (data[9] >> 2)) & 0x07ff;
	result[7] = ((data[11] << 3) + (data[10] >> 5)) & 0x07ff;
	result[8] = ((data[13] << 8) + (data[12])) & 0x07ff;
	result[9] = ((data[14] << 5) + (data[13] >> 3)) & 0x07ff;
	result[10] = ((data[16] << 10) + (data[15] << 2) + (data[14] >> 6)) & 0x07ff;
	result[11] = ((data[17] << 7) + (data[16] >> 1)) & 0x07ff;
	result[12] = ((data[18] << 4) + (data[17] >> 4)) & 0x07ff;
	result[13] = ((data[20] << 9) + (data[19] << 1) + (data[18] >> 7)) & 0x07ff;
	result[14] = ((data[21] << 6) + (data[20] >> 2)) & 0x07ff;
	result[15] = ((data[22] << 3) + (data[21] >> 5)) & 0x07ff;
    for (int i = 0; i < 16; ++i) {
        model->SetPosition(i, result[i]);
    }
    view->SetPosition(model, currentIndex);
    return true;
}