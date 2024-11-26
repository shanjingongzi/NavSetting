#pragma once
#include <tuple>
using uint8_t = unsigned char;
class Command
{
public:
    static std::tuple<uint8_t, uint8_t> XOR(unsigned char* prx_data, unsigned char len);
    inline static unsigned char requestSendQueueCmd[]   = {0xAA, 0x00, 0x01, 0x01, 0x07, 0x02};
    inline static unsigned char requestReverseCmd[]     = {0xAA, 0x00, 0x02, 0x01, 0x03, 0x03};
    inline static unsigned char requestMaximalHelmCmd[] = {0xAA, 0x00, 0x03, 0x01, 0x0B, 0x02};

    template<int N>
    static std::tuple<uint8_t, uint8_t> Check(uint8_t data[N])
    {
        if (N < 6) {
            return {};
        }
        return XOR(data + 2, N - 2 - 2);
    }

private:
};