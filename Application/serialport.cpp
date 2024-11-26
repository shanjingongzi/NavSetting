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
    impl->Open();
}
bool SerialPort::Close()
{
    impl->Close();
}
QByteArray SerialPort::Read()
{
    return impl->Read();
}
void SerialPort::Write(unsigned char* byte, int length)
{
    impl->Write(byte, length);
}