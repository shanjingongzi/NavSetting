#ifndef NAVSETTINGVIEW_H
#define NAVSETTINGVIEW_H
#include "navsettingmodel.h"
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qprogressbar.h>
#include <qspinbox.h>
#include <qtmetamacros.h>
#include <qwidget.h>


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
    void SetModel(NavSettingModel* model);
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
    void RequestReverse();
    void RequestMaximalHelm();
    void RequestSendQueue();
    void WriteSignalSource();
    void WriteReverse();
    void WriteMinimalHelm();
    void WriteMaximalHelm();
    void WriteMiddleHelm();

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
};

#endif   // NAVSETTINGVIEW_H
