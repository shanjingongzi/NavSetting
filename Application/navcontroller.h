#ifndef NAVCONTROLLER_H
#define NAVCONTROLLER_H
#include <QObject>
#include <qtmetamacros.h>
#include <qwidget.h>

#include "navsettingmodel.h"
#include "navsettingview.h"
#include "serialport.h"
#include <map>

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
    
signals:
    void MessageChanged(const QString& msg);

private:
    NavSettingView* view;
    SerialPort* device = nullptr;
    int channelNum     = 16;
    std::atomic<bool> enableListen;
    uint8_t currentIndex = 1;
    std::map<uint8_t, NavSettingModel*> models;
};

#endif   // NAVCONTROLLER_H
