#include "widget.h"
#include "ui_widget.h"
#include "stdlib.h"
#include <qvalidator.h>
#include <qlineedit.h>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTime>
#include <QDebug>
#include <QDateTime>

Widget::Widget(QWidget *parent):
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    foreach (const QSerialPortInfo &Info, QSerialPortInfo::availablePorts()) {
        qDebug() << "Port_Name :" << Info.portName();
        qDebug() << "Description :" << Info.description();
        qDebug() << "Manufacturer :" << Info.manufacturer();
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_comopen_clicked()
{
    my_serialPort = new QSerialPort(this);
    my_serialPort->setPortName( ui->comboBox_portName->currentText() );
    my_serialPort->open( QIODevice::ReadWrite );
    qDebug() << ui->comboBox_portName->currentText();
    my_serialPort->setBaudRate(  ui->comboBox_baudRate->currentText().toInt() );//波特率

    //设置数据位
    switch(ui->comboBox_databits->currentText().toInt())
    {
        case 5:
             my_serialPort->setDataBits(QSerialPort::Data5);break;
        case 6:
             my_serialPort->setDataBits(QSerialPort::Data6);break;
        case 7:
             my_serialPort->setDataBits(QSerialPort::Data7);break;
        case 8:
             my_serialPort->setDataBits(QSerialPort::Data8);break;
        default: break;
    }
    //设置校验位
    switch(ui->comboBox_parity->currentIndex())
    {
        case 0:
            my_serialPort->setParity(QSerialPort::NoParity);break;
        case 1:
            my_serialPort->setParity(QSerialPort::EvenParity);break;
        case 2:
            my_serialPort->setParity(QSerialPort::OddParity);break;
        case 3:
            my_serialPort->setParity(QSerialPort::SpaceParity);break;
        case 4:
            my_serialPort->setParity(QSerialPort::MarkParity);break;
        default: break;
    }
    //设置流控制
    switch(ui->comboBox_FlowControl->currentIndex())
    {
        case 0:
            my_serialPort->setFlowControl(QSerialPort::NoFlowControl);
        case 1:
            my_serialPort->setFlowControl(QSerialPort::HardwareControl);
        case 2:
            my_serialPort->setFlowControl(QSerialPort::SoftwareControl);
        default: break;
    }

    //设置停止位
    switch(ui->comboBox_stopbits->currentIndex())
    {
        case 1:
            my_serialPort->setStopBits(QSerialPort::OneStop);
        case 2:
            my_serialPort->setStopBits(QSerialPort::TwoStop);
        default: break;
     }

    timer = new QTimer(this);
    connect( timer, SIGNAL( timeout() ), this, SLOT( readMyCom() ) );
    timer->start(1000);//每秒读一次
}

void Widget::on_pushButton_send_clicked()
{    
    QRegExp regExp("[A-Fa-f0-9]{1,1000}");
    QValidator *validator = new QRegExpValidator(regExp, this);
    ui->lineEdit_write->setValidator(validator);

    QString sendData = ui->lineEdit_write->text();
    ui->lineEdit_write->setClearButtonEnabled(true);

    //HexToString
    QByteArray sendbuff, sendarray;
    uchar tempchar;

    sendbuff = QByteArray::fromHex(sendData.toLatin1().data());
    qDebug() << "sendData :" << sendData << endl;
    qDebug() << "sendbuff :" << sendbuff << endl;

    int length = sendbuff.length();
    for(int i = 0 ; i < length; i++){
        tempchar = sendbuff.at(i) & 0xff;
        qDebug() << "tempchar :" << tempchar << endl;
        sendarray.append(tempchar);
    }

    for(int i = 0 ; i < length; i++){
        qDebug() << "sendarray[] :" << sendarray.at(i) << endl;
    }

    qDebug() << "sendbuff_length: " << length << endl;
    qDebug() << "sendarray :" << sendarray << endl;
    my_serialPort->write(sendarray);
    sendarray.clear();
}

void Widget::readMyCom()//读取缓冲的数据，每秒读一次
{
    //Current Time
    QDateTime qdatatime = QDateTime::currentDateTime();
    QString strTime = qdatatime.toString("yyyy.MM.dd hh:m:s.zzz"); 

    //用requestData存储从串口那读取的数据
    QByteArray requestData = my_serialPort->readAll();
    int length = requestData.length();
    if(length != 0 )
    {
        int len = requestData.size();
        for(int i = 0 ; i < len ; i++){
            uchar uchar_data = requestData.at(i) & 0xff;
            qDebug() << "requestData.at(" << i << "): "<< uchar_data << endl;
            if(uchar_data != 192){
                read_buff.append(uchar_data);
            }

            else{
                if(read_buff != NULL){
                    QByteArray read_buff_hex = read_buff.toHex();
                    int len = read_buff_hex.length();
                    qDebug() << "read_buff_hex: " << read_buff_hex << endl;
                    ui->textEdit_read->append('[' + strTime + ']');
                    ui->textEdit_read->append(read_buff_hex);
                    ui->textEdit_read->append("\n");
                    read_buff.clear();
            }
        }
    }
    requestData.clear();    //清除缓冲区
    }
}

void Widget::on_pushButton_sendarea_clear_clicked()
{
    ui->lineEdit_write->clear();
}

void Widget::on_pushButton_receivearea_clear_clicked()
{
    ui->textEdit_read->clear();
}
