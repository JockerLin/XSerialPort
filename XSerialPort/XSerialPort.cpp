#pragma execution_character_set("utf-8")
#include "XSerialPort.h"

XSerialPort::XSerialPort(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // 端口号
    findSerialPort();
    // 波特率
    QStringList sListBaudRate = {"115200","57600","56000","38400","19200","14400" ,"9600","4800","2400","1200","600","300","110"};
    ui.comboBox_BaudRate->addItems(sListBaudRate);
    // 校验位
    QStringList parityList;
    parityList << "None" << "Odd" << "Even";
    ui.comboBox_Parity->addItems(parityList);
    // 数据位
    QStringList dataBits;
    dataBits << "8" << "7" << "6" << "5";
    ui.comboBox_DataBit->addItems(dataBits);
    // 停止位
    QStringList stopBits;
    stopBits << "1" << "2";
    ui.comboBox_StopBit->addItems(stopBits);

    auto listCmbs = findChildren<QComboBox*>();
    for each (QComboBox* cmb in listCmbs)
    {
        connect(cmb, &QComboBox::currentTextChanged, this, &XSerialPort::onComboboxTextChange);
    }

    auto listBtns = findChildren<QPushButton*>();
    for each (QPushButton * btn in listBtns)
    {
        connect(btn, &QPushButton::clicked, this, &XSerialPort::onPushbuttonClicked);
    }

    serialPortImp = new QSerialPort();

    // 接受串口数据
    QObject::connect(serialPortImp, &QSerialPort::readyRead, this, [&]() {
        auto RxData = serialPortImp->readAll();
        auto ReceivDatSize = RxData.size();//统计接收字符大小
        nCountRevByts += ReceivDatSize;
        if (RxData.isEmpty())//如果没有数据退出减少吗、内存占用
        {
            return;
        }
        QString strReceive = QString::fromLocal8Bit(RxData);
        ui.textEdit_receive->append(strReceive);
        ui.label_recByts->setText(QString::number(nCountRevByts));
    });
}

XSerialPort::~XSerialPort()
{}

void XSerialPort::findSerialPort()
{
    ui.comboBox_Port->blockSignals(true);
    QString _lastText = ui.comboBox_Port->currentText();
    ui.comboBox_Port->clear();

    auto _infos = QSerialPortInfo::availablePorts();
    QStringList _listPorts;
    for each (const QSerialPortInfo& info in _infos)
    {
        _listPorts.push_back(info.portName());
    }
    ui.comboBox_Port->addItems(_listPorts);

    if (!_lastText.isEmpty() && _listPorts.contains(_lastText))
    {
        ui.comboBox_Port->setCurrentText(_lastText);
    }
    /*for (int i = 0; i < _infos.size(); i++)
    {
        ui.comboBox_Port->addItem(_infos[i].portName());
    }*/

    ui.comboBox_Port->blockSignals(false);
}

void XSerialPort::openSerial()
{
    // 打开
    if (xSerialInfo.connected)
    {
        return;
    }

    QString sBaudRate = ui.comboBox_BaudRate->currentText();
    QString sPortName = ui.comboBox_Port->currentText();
    QString parity = ui.comboBox_Parity->currentText();
    QString dataBit = ui.comboBox_DataBit->currentText();
    QString stopBit = ui.comboBox_StopBit->currentText();

    serialPortImp->setBaudRate(sBaudRate.toInt());
    serialPortImp->setPortName(sPortName);
    // parityList << "None" << "Odd" << "Even";
    QSerialPort::Parity _parity = QSerialPort::NoParity;
    if ("None" == parity)
    {
        _parity = QSerialPort::NoParity;
    }
    else if ("Odd" == parity)
    {
        _parity = QSerialPort::OddParity;
    }
    else if ("Even" == parity)
    {
        _parity = QSerialPort::EvenParity;
    }
    serialPortImp->setParity(_parity);

    QSerialPort::DataBits _dataBit = QSerialPort::DataBits(dataBit.toInt());
    serialPortImp->setDataBits(_dataBit);
    QSerialPort::StopBits _stopBit = QSerialPort::StopBits(stopBit.toInt());
    serialPortImp->setStopBits(_stopBit);

    if (!serialPortImp->open(QIODevice::ReadWrite))
    {
        ui.textEdit_send->append("串口打开失败！");
        return;
    }

    ui.textEdit_send->append("串口打开成功！");

    xSerialInfo.baudRate = sBaudRate;
    xSerialInfo.portNum = sPortName;
    xSerialInfo.parity = parity;
    xSerialInfo.dataBit = dataBit;
    xSerialInfo.stopBit = stopBit;
    xSerialInfo.connected = true;
    ui.pushButton_open->setText("Close");

    ui.groupBox->setEnabled(false);

    // 其他设置
    //3、设置波特率
    //serialPortImp->setBaudRate(38400);//38400
    //4、设置校验位
    //serialPortImp->setParity(QSerialPort::NoParity);
    //5、设置数据位
    //serialPortImp->setDataBits(QSerialPort::Data8);
    //6、设置停止位  
    /*serialPortImp->setStopBits(QSerialPort::OneStop);*/
    //7、设置流控制  
    serialPortImp->setFlowControl(QSerialPort::NoFlowControl);
}

void XSerialPort::closeSerial()
{
    serialPortImp->close();
    xSerialInfo.connected = false;
    ui.pushButton_open->setText("Open");

    ui.groupBox->setEnabled(true);

    ui.textEdit_send->append("串口关闭成功！");
}

void XSerialPort::onComboboxTextChange(QString text)
{
    auto cmb = qobject_cast<QComboBox*>(sender());

    
}

void XSerialPort::onPushbuttonClicked(bool status)
{
    auto btn = qobject_cast<QPushButton*>(sender());

    /*if (ui.pushbu)
    {

    }*/
    
    if (ui.pushButton_reFlash == btn)
    {
        findSerialPort();
    }
    else if (ui.pushButton_open == btn)
    {
        if (ui.pushButton_open->text() == "Open")
        {
            openSerial();
        }
        else if (ui.pushButton_open->text() == "Close")
        {
            closeSerial();
        }
    }
    else if (ui.pushButton_send == btn)
    {
        if (nullptr!=serialPortImp && xSerialInfo.connected)
        {
            auto strSend = ui.textEdit->toPlainText();
            QByteArray bytes = strSend.toLatin1();
            nCountSendByts += bytes.size();

            serialPortImp->write(strSend.toLocal8Bit().data());
            ui.textEdit_send->append(strSend);
            ui.label_sendByts->setText(QString::number(nCountSendByts));
        }
    }
    else if (ui.pushButton_clearRecData == btn)
    {
        nCountRevByts = 0;
        ui.textEdit_receive->clear();
        ui.label_recByts->setText("");
    }
    else if (ui.pushButton_clearSendData == btn)
    {
        nCountSendByts = 0;
        ui.textEdit_send->clear();
        ui.label_sendByts->setText("");
    }
}
