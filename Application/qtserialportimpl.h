#ifndef QTSERIALPORTIMPORT_H
#define QTSERIALPORTIMPORT_H
#include "SerialportImpl.h"
#include <QSerialPort>
#include <qserialport.h>

class QtSerialPortImpl : public SerialportImpl
{
public:
    QtSerialPortImpl(const QString& name, int baud, int parityBit, int dataBit, int stopBit);
    ~QtSerialPortImpl();
    bool Open() override;
    bool Close() override;
    QByteArray Read() override;
    void Write(const unsigned char* data, int length) override;

private:
    QSerialPort* impl;
    QString name;
    int baud;
    QSerialPort::Parity parityBit;
    QSerialPort::DataBits dataBit;
    QSerialPort::StopBits stopBit;
};

#endif   // QTSERIALPORTIMPORT_H
