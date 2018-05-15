#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "rcLib.hpp"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    serial = new QSerialPort(ui->portEdit->text());
    serial->setBaudRate(ui->baudSpin->value());

    if(!serial->open(QSerialPort::ReadWrite)) {
        qDebug() << "Could not open serial port";
    } else {
        connect(serial, SIGNAL(readyRead()), this, SLOT(serialRead()));
    }
}

void MainWindow::serialRead()
{
    static rcLib::Package pkgInNew;
    QByteArray data = serial->readAll();

    for(int c=0; c<data.length(); c++){
        if(pkgInNew.decode(data[c])){
            if(pkgInNew.getDeviceId() == ui->spinTransmitter->value()) {
                ui->channelBrowser->clear();
                for(int c=0; c<pkgInNew.getChannelCount(); c++) {
                    ui->channelBrowser->append(QString::number(c) + ":\t" +
                                               QString::number(pkgInNew.getChannel(c)) + "\n");
                }
            }
        }
    }
}
