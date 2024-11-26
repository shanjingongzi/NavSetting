#pragma once
#include <QByteArray>

class SerialportImpl
{
public:
    virtual ~SerialportImpl() {}
    virtual bool Open()                                       = 0;
    virtual bool Close()                                      = 0;
    virtual QByteArray Read()                                 = 0;
    virtual void Write(const unsigned char* data, int length) = 0;
};