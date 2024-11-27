#include "qtserialportimpl.h"
#include <qserialport.h>
#include <qstringview.h>


QtSerialPortImpl::QtSerialPortImpl(const QString& name, int baud, int parityBit, int dataBit, int stopBit)
    : impl(new QSerialPort())
    , name{name}
    , baud{baud}
    , parityBit{(QSerialPort::Parity)parityBit}
    , dataBit{(QSerialPort::DataBits)dataBit}
    , stopBit{(QSerialPort::StopBits)stopBit}
{
    impl->setPortName(this->name);
    impl->setBaudRate(this->baud);
    impl->setParity(this->parityBit);
    impl->setDataBits(this->dataBit);
    impl->setStopBits(this->stopBit);
}

QtSerialPortImpl::~QtSerialPortImpl()
{
    if (impl->isOpen()) {
        impl->close();
    }
    delete impl;
}

bool QtSerialPortImpl::Open()
{
    impl->open(QIODevice::ReadWrite);
    return impl->isOpen();
}
bool QtSerialPortImpl::Close()
{
    if (impl->isOpen()) {
        impl->close();
    }
    return !impl->isOpen();
}
QByteArray QtSerialPortImpl::Read()
{
    if (impl->isOpen()) {
        return impl->readAll();
    }
    return {};
}
bool QtSerialPortImpl::Write(const unsigned char* info, int length)
{
    if (impl->isOpen()) {
        QByteArray data((const char*)info, length);
        auto result   = impl->write((const char*)info, length);
        return result = length;
    }
    return false;
}
bool QtSerialPortImpl::Write(const QByteArray& data)
{
    if (impl->isOpen()) {
        auto result = impl->write(data);
        return result == data.size();
    }
    return false;
}