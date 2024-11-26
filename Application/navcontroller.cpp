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
    view  = new NavSettingView(16, parent);
    model = new NavSettingModel(16);
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
        if (device) {
            device->Write(Command::requestReverseCmd, sizeof(Command::requestReverseCmd));
        }
    });
    connect(view, &NavSettingView::RequestMaximalHelm, [this]() {
        if (device) {
            auto check = Command::Check<sizeof(Command::requestSendQueueCmd)>(Command::requestSendQueueCmd);
            device->Write(Command::requestMaximalHelmCmd, sizeof(Command::requestMaximalHelmCmd));
        }
    });
    connect(view, &NavSettingView::RequestReverse, [this]() {
        if (device) {
            device->Write(Command::requestSendQueueCmd, sizeof(Command::requestSendQueueCmd));
        }
    });
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