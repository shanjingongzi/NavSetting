#include "navcontroller.h"
#include "QtSerialPort"
#include "navsettingmodel.h"
#include "navsettingview.h"
#include "qtserialportimpl.h"
#include "serialport.h"
#include <atomic>
#include <chrono>
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
}

QWidget* NavController::View()
{
    return view;
}

void NavController::StartListen()
{
    std::thread listenTask([this]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (!enableListen.load(std::memory_order_relaxed)) {
                break;
            }
            device->Read();
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