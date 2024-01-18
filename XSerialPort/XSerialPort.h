#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_XSerialPort.h"
#include <qserialport.h>
#include <qstringlist.h>
#include <qserialportinfo.h>

class XSerialPort : public QMainWindow
{
    Q_OBJECT

public:
    XSerialPort(QWidget *parent = nullptr);
    ~XSerialPort();

    struct XSerialInfo
    {
        QString portNum="";
        QString baudRate="";
        QString parity = "";
        QString dataBit = "";
        QString stopBit = "";
        bool connected=false;
    }xSerialInfo;

    QSerialPort* serialPortImp;

    // �Զ�Ѱ�ҵ�ǰ����
    void findSerialPort();
    // ����
    void openSerial();
    void closeSerial();

    int nCountSendByts = 0, nCountRevByts = 0;

public slots:
    void onComboboxTextChange(QString text);
    void onPushbuttonClicked(bool status);
private:
    Ui::XSerialPortClass ui;
};
