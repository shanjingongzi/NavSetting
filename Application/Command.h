#pragma once
#include "navsettingmodel.h"
#include <QByteArray>
#include <qstringview.h>
#include <tuple>


using uint8_t = unsigned char;
class Command
{
public:
    static QByteArray GenerateRequestSendQueueCmd(uint8_t sbus);
    static QByteArray GenerateQequestReverseCmd(uint8_t sbus);
    static QByteArray GenerateMaximalHelCmd(uint8_t subs);
    static QByteArray GenerateMappSlotCmd(uint8_t sbus, NavSettingModel* model);
    static QByteArray GenerateReverseCmd(uint8_t sbus, NavSettingModel* model);
    static QByteArray GenerateMinimalHelmCmd(uint8_t sbus, NavSettingModel* model);
    static QByteArray GenerateMiddleHelmCmd(uint8_t sbus, NavSettingModel* model);
    static QByteArray GenerateMaximalHelmCmd(uint8_t sbus, NavSettingModel* model);

    static QByteArray GenericWriteCmd(uint8_t cmd, uint8_t ctr);
    static void ParityCmd(QByteArray& data);

    static std::tuple<uint8_t, uint8_t> XOR(unsigned char* prx_data, unsigned char len);
    template<int N>
    static std::tuple<uint8_t, uint8_t> XOR(uint8_t data[N])
    {
        if (N < 6) {
            return {};
        }
        return XOR(data + 2, N - 2 - 2);
    }

    inline static uint8_t signalSource     = 1;
    inline static uint8_t reverse          = 2;
    inline static uint8_t maximalHelm      = 3;
    inline static uint8_t minimalHelm      = 4;
    inline static uint8_t fineTune         = 5;
    inline static uint8_t sbus1            = 1;
    inline static uint8_t sbus2            = 2;
    inline static uint8_t configSerialport = 3;
    inline static uint8_t dataBit          = 4;
    inline static uint8_t parityBit        = 30;
    inline static uint8_t cmdBit           = 2;
    inline static uint8_t ctrBit           = 3;

    inline static unsigned char requestSendQueueCmd[]   = {0xAA, 0x00, 0x01, 0x01, 0x07, 0x02};
    inline static unsigned char requestReverseCmd[]     = {0xAA, 0x00, 0x02, 0x01, 0x03, 0x03};
    inline static unsigned char requestMaximalHelmCmd[] = {0xAA, 0x00, 0x03, 0x01, 0x0B, 0x02};
    inline static unsigned char mappSlotCmd[]           = {0xAA, 0x55, 0x01, 0x01, 0x11, 0x00, 0x12, 0x00, 0x13, 0x00, 0x14, 0x00, 0x15, 0x00, 0x16, 0x00, 0x17, 0x00, 0x18,
                                                           0x00, 0x19, 0x00, 0x1A, 0x00, 0x1B, 0x00, 0x1C, 0x00, 0x1D, 0x00, 0x1E, 0x00, 0x1F, 0x00, 0x20, 0x00, 0xD7, 0x30};

    inline static unsigned char write_template[38] = {0xAA, 0x55,
                                                      /*command bit*/
                                                      0x00,
                                                      /*control bit*/
                                                      0x00,
                                                      /*data bit*/
                                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                      /*parity*/
                                                      0x00, 0x00};



private:
};