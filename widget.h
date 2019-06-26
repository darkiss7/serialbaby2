#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QDebug>

#include <qvalidator.h>
#include <qlineedit.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pushButton_comopen_clicked();
    void on_pushButton_send_clicked();
    void readMyCom();
    void on_pushButton_sendarea_clear_clicked();
    void on_pushButton_receivearea_clear_clicked();

private:
    Ui::Widget *ui;
    QSerialPort *my_serialPort;
    QByteArray  requestData;
    QByteArray read_buff;
    QTimer *timer;

};

#endif // WIDGET_H
