#include "navcontroller.h"
#include "Command.h"
#include "QtSerialPort"
#include "navsettingmodel.h"
#include "navsettingview.h"
#include "qtserialportimpl.h"
#include "serialport.h"
#include <atomic>
#include <chrono>
#include <qdebug.h>
#include <thread>



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
            device->Write(Command::GenerateRequestSignalSourceCmd(currentIndex), 6);
        }
    });

    connect(view, &NavSettingView::RequestReverse, [this]() {
        if (device) {
            device->Write(Command::GenerateRequestReverseCmd(currentIndex), 6);
        }
    });
    connect(view, &NavSettingView::RequestMinimalHelm, [this]() {
        if (device) {
            device->Write(Command::GenerateRequestMinimalHelm(currentIndex), 6);
        }
    });

    connect(view, &NavSettingView::RequestMaximalHelm, [this]() {
        if (device) {
            device->Write(Command::GenerateRequestMaximalHelm(currentIndex), 6);
        }
    });

    connect(view, &NavSettingView::RequestFineTune, [this]() {
        if (device) {
            device->Write(Command::GenerateRequestFineTune(currentIndex), 6);
        }
    });

    connect(view, &NavSettingView::WriteSignalSource, [this]() {
        if (device) {
            device->Write(Command::GenerateMappSlotCmd(Command::configSerialport, Model()));
        }
    });
    connect(view, &NavSettingView::WriteReverse, [this]() {
        if (device) {
            device->Write(Command::GenerateReverseCmd(Command::configSerialport, Model()));
        }
    });
    connect(view, &NavSettingView::WriteMinimalHelm, [this]() {
        if (device) {
            device->Write(Command::GenerateMinimalHelmCmd(Command::configSerialport, Model()));
        }
    });
    connect(view, &NavSettingView::WriteMaximalHelm, [this]() {
        if (device) {
            device->Write(Command::GenerateMaximalHelmCmd(Command::configSerialport, Model()));
        }
    });
    connect(view, &NavSettingView::WriteMiddleHelm, [this]() {
        if (device) {
            device->Write(Command::GenerateMiddleHelmCmd(Command::configSerialport, Model()));
        }
    });
    connect(view, &NavSettingView::MapperChanged, [this](int channel, int val) { Model()->SetMapSlot(channel, val); });
    connect(view, &NavSettingView::ReveseChanged, [this](int channel, bool val) { Model()->SetReverse(channel, val); });
    connect(view, &NavSettingView::MinimalHelmChanged, [this](int channel, int val) { Model()->SetMinimalHelm(channel, val); });
    connect(view, &NavSettingView::MiddleHeelmChanged, [this](int channel, int val) { Model()->SetMiddlelHelm(channel, val); });
    connect(view, &NavSettingView::MaximalHelmChanged, [this](int channel, int val) { Model()->SetMaximalHelm(channel, val); });
    connect(view, &NavSettingView::sbusChanged, [this](int index) { this->currentIndex = index + 1; });
}

QWidget* NavController::View()
{
    return view;
}

void NavController::StartListen()
{
    std::thread listenTask([this]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            if (!enableListen.load(std::memory_order_relaxed)) {
                break;
            }
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
    });
    enableListen.store(true, std::memory_order_relaxed);
    listenTask.detach();
    auto id = listenTask.get_id();
}
void NavController::StopListen()
{
    enableListen.store(false, std::memory_order_relaxed);
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

    auto parseValue = [&ptr]() {
        unsigned short val = 0x0000;
        unsigned char low  = *ptr++;
        unsigned char high = *ptr++;
        val |= high;
        val = val << 8;
        val |= low;
        return val;
    };

    switch (cmdBit) {
    case Command::signalSource:
        for (int i = 0; i < channelNum; ++i) {
            auto val = parseValue();
            model->second->SetMapSlot(i, val);
        }
        break;
    case Command::reverse:
        for (int i = 0; i < channelNum; ++i) {
            auto val = parseValue();
            model->second->SetReverse(i, val);
        }
        break;
    case Command::minimalHelm:
        for (int i = 0; i < channelNum; ++i) {
            auto val = parseValue();
            model->second->SetMinimalHelm(i, val);
        }
        break;
    case Command::maximalHelm:
        for (int i = 0; i < channelNum; ++i) {
            auto val = parseValue();
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

    view->SetModel(model->second);
}
