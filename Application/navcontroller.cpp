#include "navcontroller.h"
#include "Command.h"
#include "navsettingmodel.h"
#include "navsettingview.h"
#include "qtserialportimpl.h"
#include "serialport.h"
#include <QTimerEvent>
#include <atomic>
#include <qdebug.h>


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
    connect(view, &NavSettingView::Open, [this](const QString& name) {
        if (device) {
            StopListen();
            delete device;
        }
        device = new SerialPort(new QtSerialPortImpl(name, 100000, 2, 8, 2));
        if (device->Open()) {
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
    commands.push(Command::GenerateRequestFineTune(sbus));
    commands.push(Command::GenerateRequestMaximalHelm(sbus));
    commands.push(Command::GenerateRequestMinimalHelm(sbus));
    commands.push(Command::GenerateRequestReverseCmd(sbus));
    commands.push(Command::GenerateRequestSignalSourceCmd(sbus));
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
                commands.pop();
            }
            if (!commands.empty()) {
                device->Write(commands.front());
                lastCommand = commands.front()[Command::cmdBit];
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
                qDebug() << msg;
                qDebug() << msg.size();
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
    }
    view->SetModel(model->second, currentIndex);
    respondCommand.store(cmdBit);
}
