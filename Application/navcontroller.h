#ifndef NAVCONTROLLER_H
#define NAVCONTROLLER_H
#include <QObject>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "navsettingmodel.h"
#include "navsettingview.h"
#include "serialport.h"
#include <map>
#include <queue>

class NavController : public QObject
{
    Q_OBJECT
public:
    NavController(QWidget* parent = nullptr);
    void Initialize();
    QWidget* View();
    void StartListen();
    void StopListen();
    inline NavSettingModel* Model()
    {
        auto iter = models.find(currentIndex);
        assert(iter != models.end());
        return iter->second;
    }
    void ParseRespond(const QByteArray& data);
    void Read(uint8_t sbus);
    void Request(const QByteArray &data);
    virtual void timerEvent(QTimerEvent* event)override;
signals:
    void MessageChanged(const QString& msg);


private:
    NavSettingView* view;
    SerialPort* device = nullptr;
    int channelNum     = 16;
    std::atomic<bool> enableListen;
    std::atomic<int> respondCommand;
    uint8_t currentIndex = 1;
    std::map<uint8_t, NavSettingModel*> models;
    std::queue<QByteArray>commands;
    int lastCommand;
   
};

#endif   // NAVCONTROLLER_H
