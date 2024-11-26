#ifndef NAVSETTINGMODEL_H
#define NAVSETTINGMODEL_H

#include <QVector>

struct NavItem
{
    int channel;
    int position;
    int helm;
    int mappSlot;
    bool reverse;
    int minimalHelm;
    int middleHelm;
    int maximalHelm;
};
class NavSettingModel
{
public:
    NavSettingModel(int maxChannel);
    void SetMapSlot(int channel, int val);
    int GetMapSlot(int channel);
    void SetPosition(int channel, int position);
    int GetPosition(int channel);
    void SetHelm(int channel, int val);
    int GetHelm(int channel);

    void SetReverse(int channel, bool val);
    bool GetReverse(int channel);
    void SetMinimalHelm(int channel, int val);
    int GetMinimalHelm(int channel);
    void SetMiddlelHelm(int channel, int val);
    int GetMiddleHelm(int channel);
    void SetMaximalHelm(int channel, int val);
    int GetMaximalHelm(int channel);

public:
    QVector<NavItem> data;
};

#endif   // NAVSETTINGMODEL_H
