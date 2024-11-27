#include "Command.h"
#include "navsettingmodel.h"
#include <qdir.h>
#include <qstringview.h>
#include <tuple>

std::tuple<uint8_t, uint8_t> Command::XOR(unsigned char* prx_data, unsigned char len)
{
    unsigned char X_OR, Y_OR;
    uint8_t i, a, b, c, d;
    X_OR = 0;
    Y_OR = *prx_data;
    for (i = 0; i < len; i++) {
        a = *(prx_data + i);
        d = 0;
        for (b = 0; b < 8; b++)   //
        {
            c = (uint8_t)(a & 0x01);
            d += c;
            a >>= 1;
        }
        if ((d % 2) == 1)
            X_OR <<= 1;   //
        else {
            X_OR <<= 1;
            X_OR += 1;
        }   //
        if ((i + 1) < len)
            Y_OR = (uint8_t)(Y_OR ^ *(prx_data + i + 1));
        else {
            X_OR <<= 2;
            X_OR += 3;
        }
    }
    return {X_OR, Y_OR};
}

QByteArray Command::GenericWriteCmd(uint8_t cmd, uint8_t ctr)
{
    QByteArray result((char*)write_template, sizeof(write_template));
    result[cmdBit] = cmd;
    result[ctrBit] = ctr;
    return result;
}

void Command::ParityCmd(QByteArray& data)
{
    auto result           = XOR((unsigned char*)&data[2], data.size() - 4);
    auto& [x_or, y_or]    = result;
    data[data.size() - 1] = y_or;
    data[data.size() - 2] = x_or;
}

QByteArray Command::GenerateMappSlotCmd(uint8_t sbus, NavSettingModel* model)
{
    QByteArray result = GenericWriteCmd(signalSource, sbus);
    for (int i = 0; i < model->Size(); ++i) {
        result[dataBit + i] = model->GetMapSlot(i);
    }
    ParityCmd(result);
    return result;
}

QByteArray Command::GenerateReverseCmd(uint8_t sbus, NavSettingModel* model)
{
    QByteArray result = GenericWriteCmd(reverse, sbus);
    for (int i = 0; i < model->Size(); ++i) {
        result[dataBit + i] = (unsigned char)model->GetReverse(i);
    }
    ParityCmd(result);
    return result;
}

QByteArray Command::GenerateMinimalHelmCmd(uint8_t sbus, NavSettingModel* model)
{
    QByteArray result = GenericWriteCmd(minimalHelm, sbus);
    for (int i = 0; i < model->Size(); ++i) {
        result[dataBit + i] = (model->GetMinimalHelm(i));
    }
    ParityCmd(result);
    return result;
}

QByteArray Command::GenerateMiddleHelmCmd(uint8_t sbus, NavSettingModel* model)
{
    QByteArray result = GenericWriteCmd(fineTune, sbus);
    for (int i = 0; i < model->Size(); ++i) {
        result[dataBit + i] = (model->GetMiddleHelm(i));
    }
    ParityCmd(result);
    return result;
}

QByteArray Command::GenerateMaximalHelmCmd(uint8_t sbus, NavSettingModel* model)
{
    QByteArray result = GenericWriteCmd(maximalHelm, sbus);
    for (int i = 0; i < model->Size(); ++i) {
        result[dataBit + i] = (model->GetMaximalHelm(i));
    }
    ParityCmd(result);
    return result;
}