#include "navsettingmodel.h"

NavSettingModel::NavSettingModel(int maxChannel)
    : data(maxChannel)
{}

void NavSettingModel::SetMapSlot(int channel, int val)
{
    data[channel].mappSlot = val;
}
int NavSettingModel::GetMapSlot(int channel)
{
    return data[channel].mappSlot;
}
void NavSettingModel::NavSettingModel::SetPosition(int channel, int position)
{
    data[channel].position = position;
}
int NavSettingModel::GetPosition(int channel)
{
    return data[channel].position;
}
void NavSettingModel::SetHelm(int channel, int val)
{
    data[channel].helm = val;
}
int NavSettingModel::GetHelm(int channel)
{
    return data[channel].helm;
}

void NavSettingModel::SetReverse(int channel, bool val)
{
    data[channel].reverse = val;
}
bool NavSettingModel::GetReverse(int channel)
{
    return data[channel].reverse;
}
void NavSettingModel::SetMinimalHelm(int channel, int val)
{
    data[channel].minimalHelm = val;
}
int NavSettingModel::GetMinimalHelm(int channel)
{
    return data[channel].minimalHelm;
}
void NavSettingModel::SetMiddlelHelm(int channel, int val)
{
    data[channel].middleHelm = val;
}

int NavSettingModel::GetMiddleHelm(int channel)
{
    return data[channel].middleHelm;
}
void NavSettingModel::SetMaximalHelm(int channel, int val)
{
    data[channel].maximalHelm = val;
}
int NavSettingModel::GetMaximalHelm(int channel)
{
    return data[channel].maximalHelm;
}