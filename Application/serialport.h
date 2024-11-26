#ifndef SERIALPORT_H
#define SERIALPORT_H


#include "SerialportImpl.h"
#include <qbytearrayview.h>
#include <qstringview.h>

class SerialPort
{
public:
    SerialPort(SerialportImpl* impl);
    ~SerialPort();
    bool Open();
    bool Close();
    QByteArray Read();
    void Write(unsigned char* byte, int lengh);

private:
    SerialportImpl* impl;
};

#endif   // SERIALPORT_H
