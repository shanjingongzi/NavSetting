#pragma once
#include <QByteArray>
#include <tuple>

using uint8_t = unsigned char;
class Command
{
public:
    static QByteArray GenerateRequestSendQueueCmd(uint8_t sbus);
    static QByteArray GenerateQequestReverseCmd(uint8_t sbus);
    static QByteArray GenerateMaximalHelCmd(uint8_t subs);
    static QByteArray GenerateMappSlot(uint8_t sbus);

    static std::tuple<uint8_t, uint8_t> XOR(unsigned char* prx_data, unsigned char len);
    inline static unsigned char requestSendQueueCmd[]   = {0xAA, 0x00, 0x01, 0x01, 0x07, 0x02};
    inline static unsigned char requestReverseCmd[]     = {0xAA, 0x00, 0x02, 0x01, 0x03, 0x03};
    inline static unsigned char requestMaximalHelmCmd[] = {0xAA, 0x00, 0x03, 0x01, 0x0B, 0x02};
    inline static unsigned char mappSlotCmd[]           = {0xAA, 0x55, 0x01, 0x01, 0x11, 0x00, 0x12, 0x00, 0x13, 0x00, 0x14, 0x00, 0x15, 0x00, 0x16, 0x00, 0x17, 0x00, 0x18,
                                                           0x00, 0x19, 0x00, 0x1A, 0x00, 0x1B, 0x00, 0x1C, 0x00, 0x1D, 0x00, 0x1E, 0x00, 0x1F, 0x00, 0x20, 0x00, 0xD7, 0x30};
    template<int N>
    static std::tuple<uint8_t, uint8_t> XOR(uint8_t data[N])
    {
        if (N < 6) {
            return {};
        }
        return XOR(data + 2, N - 2 - 2);
    }

private:
};