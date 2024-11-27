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
    view  = new NavSettingView(32, parent);
    model = new NavSettingModel(32);
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
        device->Open();
        StartListen();
    });
    connect(view, &NavSettingView::Close, [this]() {
        if (device) {
            device->Close();
            StopListen();
        }
    });
    connect(view, &NavSettingView::StopListen, [this]() { this->StopListen(); });

    connect(view, &NavSettingView::RequestReverse, [this]() {
        if (device) {}
    });
    connect(view, &NavSettingView::RequestMaximalHelm, [this]() {
        if (device) {
            device->Write(Command::requestMaximalHelmCmd, sizeof(Command::requestMaximalHelmCmd));
        }
    });
    connect(view, &NavSettingView::RequestSendQueue, [this]() {
        if (device) {
            device->Write(Command::requestSendQueueCmd, sizeof(Command::requestSendQueueCmd));
        }
    });
    connect(view, &NavSettingView::WriteSignalSource, [this]() {
        if (device) {
            device->Write(Command::GenerateMappSlotCmd(Command::sbus1, model));
        }
    });
    connect(view, &NavSettingView::WriteReverse, [this]() {
        if (device) {
            device->Write(Command::GenerateReverseCmd(Command::sbus1, model));
        }
    });
    connect(view, &NavSettingView::WriteMinimalHelm, [this]() {
        if (device) {
            device->Write(Command::GenerateMinimalHelmCmd(Command::sbus1, model));
        }
    });
    connect(view, &NavSettingView::WriteMaximalHelm, [this]() {
        if (device) {
            device->Write(Command::GenerateMaximalHelmCmd(Command::sbus1, model));
        }
    });
    connect(view, &NavSettingView::WriteMiddleHelm, [this]() {
        if (device) {
            device->Write(Command::GenerateMiddleHelmCmd(Command::sbus1, model));
        }
    });
    connect(view, &NavSettingView::MapperChanged, [this](int channel, int val) { model->SetMapSlot(channel, val); });
    connect(view, &NavSettingView::ReveseChanged, [this](int channel, bool val) { model->SetReverse(channel, val); });
    connect(view, &NavSettingView::MinimalHelmChanged, [this](int channel, int val) { model->SetMinimalHelm(channel, val); });
    connect(view, &NavSettingView::MiddleHeelmChanged, [this](int channel, int val) { model->SetMiddlelHelm(channel, val); });
    connect(view, &NavSettingView::MaximalHelmChanged, [this](int channel, int val) { model->SetMaximalHelm(channel, val); });
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
            auto msg = device->Read().toHex(' ');
            emit MessageChanged(msg);
            qDebug() << msg;
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