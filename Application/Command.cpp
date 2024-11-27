#include "Command.h"
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

QByteArray Command::GenerateMappSlot(uint8_t sbus)
{
    QByteArray result((char*)mappSlotCmd, sizeof(mappSlotCmd));
    auto parity               = XOR<sizeof(mappSlotCmd)>(mappSlotCmd);
    const auto& [x_or, y_or]  = parity;
    result[result.size() - 1] = x_or;
    result[result.size() - 2] = y_or;
    return result;
}