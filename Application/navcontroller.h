#ifndef NAVCONTROLLER_H
#define NAVCONTROLLER_H
#include <QObject>
#include <qwidget.h>

#include "navsettingmodel.h"
#include "navsettingview.h"
#include "serialport.h"

class NavController : public QObject
{
public:
    NavController(QWidget* parent = nullptr);
    void Initialize();
    QWidget* View();
    void StartListen();
    void StopListen();

private:
    NavSettingView* view;
    NavSettingModel* model;
    SerialPort* device = nullptr;
    int channelNum     = 16;
    std::atomic<bool> enableListen;
};

#endif   // NAVCONTROLLER_H
