#ifndef NAVSETTINGVIEW_H
#define NAVSETTINGVIEW_H
#include "navsettingmodel.h"
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>
#include <map>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qobject.h>
#include <qprogressbar.h>
#include <qspinbox.h>
#include <qtmetamacros.h>
#include <qwidget.h>


struct SignalLock
{
    SignalLock(QObject* obj)
        : obj{obj}
    {
        obj->blockSignals(true);
    }
    ~SignalLock() { obj->blockSignals(false); }
    QObject* obj;
};

class NavSettingView : public QWidget
{
    Q_OBJECT
public:
    NavSettingView(int channelNum, QWidget* parent = nullptr);

    void Initialize();
    void InitializeTopBar();
    void InitializeDebug();
    void InitializeChannelPanel();
    void InitializeBottomBar();
    void InitializeAdSettingPanel();
    void Connect();
    void SetModel(NavSettingModel* model,uint8_t);
signals:
    void Open(const QString& name);
    void Close();
    void Read();
    void Write();
    void Calibration();
    void MapperChanged(int channel, int mapSlot);
    void ReveseChanged(int channel, bool val);
    void MinimalHelmChanged(int channel, int val);
    void MiddleHeelmChanged(int channel, int val);
    void MaximalHelmChanged(int channel, int val);
    void StopListen();

    void RequestSignalSource();
    void RequestReverse();
    void RequestMinimalHelm();
    void RequestMaximalHelm();
    void RequestFineTune();


    void WriteSignalSource();
    void WriteReverse();
    void WriteMinimalHelm();
    void WriteMaximalHelm();
    void WriteMiddleHelm();
    void sbusChanged(int index);

private:
    struct ChannelItem
    {
        QProgressBar* position;
        QSpinBox* helm;
        QComboBox* mapper;
    };
    struct AdSettingItem
    {
        QCheckBox* reverse;
        QSpinBox* minHelm;
        QSpinBox* midHelm;
        QSpinBox* maxHelm;
    };

private:
    QVBoxLayout* layout;
    int channelNum = 16;
    QVector<ChannelItem> channelItems;
    QVector<AdSettingItem> adSettingItems;
    std::map<int, NavSettingModel*> models;
    int currentIndex = 0;
};

#endif   // NAVSETTINGVIEW_H
