#ifndef NAVCONTROLLER_H
#define NAVCONTROLLER_H
#include <QObject>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "navsettingmodel.h"
#include "navsettingview.h"
#include "serialport.h"

class NavController : public QObject
{
    Q_OBJECT
public:
    NavController(QWidget* parent = nullptr);
    void Initialize();
    QWidget* View();
    void StartListen();
    void StopListen();
    signals:
    void MessageChanged(const QString &msg);

private:
    NavSettingView* view;
    NavSettingModel* model;
    SerialPort* device = nullptr;
    int channelNum     = 16;
    std::atomic<bool> enableListen;
};

#endif   // NAVCONTROLLER_H
