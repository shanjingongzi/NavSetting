#pragma once
#include "navsettingmodel.h"
#include <QByteArray>
#include <qstringview.h>
#include <tuple>


using uint8_t = unsigned char;
class Command
{
public:
    static uint8_t* GenerateRequestSignalSourceCmd(uint8_t sbus);
    static uint8_t* GenerateRequestReverseCmd(uint8_t sbus);
    static uint8_t* GenerateRequestMinimalHelm(uint8_t sbus);
    static uint8_t* GenerateRequestMaximalHelm(uint8_t sbus);
    static uint8_t* GenerateRequestFineTune(uint8_t sbus);

    static QByteArray GenerateMaximalHelCmd(uint8_t subs);
    static QByteArray GenerateMappSlotCmd(uint8_t sbus, NavSettingModel* model);
    static QByteArray GenerateReverseCmd(uint8_t sbus, NavSettingModel* model);
    static QByteArray GenerateMinimalHelmCmd(uint8_t sbus, NavSettingModel* model);
    static QByteArray GenerateMiddleHelmCmd(uint8_t sbus, NavSettingModel* model);
    static QByteArray GenerateMaximalHelmCmd(uint8_t sbus, NavSettingModel* model);

    static QByteArray GenericWriteCmd(uint8_t cmd, uint8_t ctr);
    static QByteArray GenericReadCmd(uint8_t cmd, uint8_t ctr);
    static void ParityCmd(QByteArray& data);
    static bool ParityRespond(const QByteArray& data);
    static bool ParseRespond(const QByteArray& data, uint8_t& outCmdBit, uint8_t& outCtrBit, QByteArray& outBuffer);

    static std::tuple<uint8_t, uint8_t> XOR(const unsigned char* prx_data, unsigned char len);
    template<int N>
    static std::tuple<uint8_t, uint8_t> XOR(uint8_t data[N])
    {
        if (N < 6) {
            return {};
        }
        return XOR(data + 2, N - 2 - 2);
    }


    constexpr static uint8_t signalSource     = 1;
    constexpr static uint8_t reverse          = 2;
    constexpr static uint8_t maximalHelm      = 3;
    constexpr static uint8_t minimalHelm      = 4;
    constexpr static uint8_t fineTune         = 5;
    constexpr static uint8_t sbus1            = 1;
    constexpr static uint8_t sbus2            = 2;
    constexpr static uint8_t configSerialport = 3;
    constexpr static uint8_t dataBit          = 4;
    constexpr static uint8_t parityBit        = 30;
    constexpr static uint8_t cmdBit           = 2;
    constexpr static uint8_t ctrBit           = 3;

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

    inline static unsigned char read_template[6] = {0xAA, 0x00,
                                                    // command bit
                                                    0x00,
                                                    // ctr bit
                                                    0x00,
                                                    // parity
                                                    0x00, 0x00};

    inline static unsigned char respond_head[] = {0x55, 0xAA};


private:
};