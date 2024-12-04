#include "Command.h"
#include "navsettingmodel.h"
#include <cstdint>
#include <cstring>
#include <qdir.h>
#include <qstringview.h>
#include <tuple>

std::tuple<uint8_t, uint8_t> Command::XOR(const unsigned char* prx_data, unsigned char len)
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
        int index     = dataBit + i * 2;
        result[index] = model->GetMapSlot(i);
    }
    ParityCmd(result);
    return result;
}

QByteArray Command::GenerateReverseCmd(uint8_t sbus, NavSettingModel* model)
{
    QByteArray result = GenericWriteCmd(reverse, sbus);
    for (int i = 0; i < model->Size(); ++i) {
        int index     = dataBit + i * 2;
        result[index] = (unsigned char)model->GetReverse(i);
    }
    ParityCmd(result);
    return result;
}

QByteArray Command::GenerateMinimalHelmCmd(uint8_t sbus, NavSettingModel* model)
{
    QByteArray result = GenericWriteCmd(minimalHelm, sbus);
    for (int i = 0; i < model->Size(); ++i) {
        int index     = dataBit + i * 2;
        unsigned short val = model->GetMinimalHelm(i);
        uint8_t low = (uint8_t)val;
        uint8_t high = (uint8_t)(val >> 8);
        result[index] = low;
        result[index + 1] = high;
    }
    ParityCmd(result);
    return result;
}

QByteArray Command::GenerateMiddleHelmCmd(uint8_t sbus, NavSettingModel* model)
{
    QByteArray result = GenericWriteCmd(fineTune, sbus);
    for (int i = 0; i < model->Size(); ++i) {
        int index     = dataBit + i * 2;
        short val = model->GetMiddleHelm(i);
        uint8_t low = (uint8_t)val;
        char high = (char)(val >> 8);
        result[index] = (low);
        result[index+1] = (high);
    }
    ParityCmd(result);
    return result;
}

QByteArray Command::GenerateMaximalHelmCmd(uint8_t sbus, NavSettingModel* model)
{
    QByteArray result = GenericWriteCmd(maximalHelm, sbus);
    for (int i = 0; i < model->Size(); ++i) {
        int index     = dataBit + i * 2;
        unsigned short val = model->GetMaximalHelm(i);
        uint8_t low = (uint8_t)val;
        uint8_t high = (uint8_t)(val >> 8);
        result[index] = low;
        result[index + 1] = high;
    }
    ParityCmd(result);
    return result;
}

uint8_t* Command::GenerateRequestSignalSourceCmd(uint8_t sbus)
{
    auto result = new uint8_t[sizeof(read_template)];
    memcpy(result, read_template, 6);
    result[cmdBit] = signalSource;
    result[ctrBit] = sbus;
    auto parity    = XOR(result + 2, 2);
    result[4]      = std::get<0>(parity);
    result[5]      = std::get<1>(parity);
    return result;
}
uint8_t* Command::GenerateRequestReverseCmd(uint8_t sbus)
{
    auto result = new uint8_t[sizeof(read_template)];
    memcpy(result, read_template, 6);
    result[cmdBit] = reverse;
    result[ctrBit] = sbus;
    auto parity    = XOR(result + 2, 2);
    result[4]      = std::get<0>(parity);
    result[5]      = std::get<1>(parity);
    return result;
}

uint8_t* Command::GenerateRequestMinimalHelm(uint8_t sbus)
{
    auto result = new uint8_t[sizeof(read_template)];
    memcpy(result, read_template, 6);
    result[cmdBit] = minimalHelm;
    result[ctrBit] = sbus;
    auto parity    = XOR(result + 2, 2);
    result[4]      = std::get<0>(parity);
    result[5]      = std::get<1>(parity);
    return result;
}

uint8_t* Command::GenerateRequestMaximalHelm(uint8_t sbus)
{
    auto result = new uint8_t[sizeof(read_template)];
    memcpy(result, read_template, 6);
    result[cmdBit] = maximalHelm;
    result[ctrBit] = sbus;
    auto parity    = XOR(result + 2, 2);
    result[4]      = std::get<0>(parity);
    result[5]      = std::get<1>(parity);
    return result;
}

uint8_t* Command::GenerateRequestFineTune(uint8_t sbus)
{
    auto result = new uint8_t[sizeof(read_template)];
    memcpy(result, read_template, 6);
    result[cmdBit] = fineTune;
    result[ctrBit] = sbus;
    auto parity    = XOR(result + 2, 2);
    result[4]      = std::get<0>(parity);
    result[5]      = std::get<1>(parity);
    return result;
}

bool Command::ParityRespond(const QByteArray& data)
{
    if (data.size() < 38) {
        return false;
    }
    int a = (uchar)data[0];
    int b = (uchar)data[1];
    if ((uchar)data[0] != 0x55 || (uchar)data[1] != 0xAA) {
        return false;
    }
    const unsigned char* ptr = (unsigned char*)data.data() + 2;
    auto parity              = XOR((const unsigned char*)(ptr), 38 - 4);
    uint8_t x_or             = std::get<0>(parity);
    uint8_t y_or             = std::get<1>(parity);
    return true;
    return data[data.size() - 1] == y_or && data[data.size() - 2] == x_or;
}