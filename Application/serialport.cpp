#include "serialport.h"

SerialPort::SerialPort(SerialportImpl* impl)
    : impl{impl}
{}
SerialPort::~SerialPort()
{
    delete impl;
}
bool SerialPort::Open()
{
    return impl->Open();
}
bool SerialPort::Close()
{
    return impl->Close();
}
QByteArray SerialPort::Read()
{
    return impl->Read();
}
bool SerialPort::Write(unsigned char* byte, int length)
{
    return impl->Write(byte, length);
}
bool SerialPort::Write(const QByteArray& array)
{
    return impl->Write(array);
}