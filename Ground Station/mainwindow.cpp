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
        ui->log->append("Could not open serial port!");
    } else {
        connect(serial, SIGNAL(readyRead()), this, SLOT(serialRead()));
        ui->log->append("Connected!");
    }
}

void MainWindow::serialRead()
{
    static rcLib::Package pkgInNew;
    QByteArray data = serial->readAll();

    for(int c=0; c<data.length(); c++){
        if(pkgInNew.decode(data[c])){
            switch (pkgInNew.getDeviceId()) {
                case 17: // lora-remote
                    ui->log->append("New Package from LoRa-Remote");
                    ui->lora0->setValue(pkgInNew.getChannel(0)-127);
                    ui->lora1->setValue(pkgInNew.getChannel(1)-127);
                    ui->lora2->setValue(pkgInNew.getChannel(2)-127);
                    ui->lora3->setValue(pkgInNew.getChannel(3)-127);
                    ui->lora4->setValue(pkgInNew.getChannel(4));
                    ui->lora5->setValue(pkgInNew.getChannel(5));
                    ui->lora6->setValue(pkgInNew.getChannel(6));
                    ui->lora7->setValue(pkgInNew.getChannel(7));
                break;
                case 23: // flight-controller
                    ui->log->append("New Package from Flight-Controller");
                    ui->fc0->setValue(pkgInNew.getChannel(0));
                    ui->fc1->setValue(pkgInNew.getChannel(1)-180);
                    ui->fc2->setValue(pkgInNew.getChannel(2)-180);
                    ui->fc3->setValue(pkgInNew.getChannel(3));
                    ui->fc4->setValue(pkgInNew.getChannel(4));
                    ui->fc5->setValue(pkgInNew.getChannel(5));
                    ui->fc6->setValue(pkgInNew.getChannel(6)-500);
                    ui->fc7->setValue(pkgInNew.getChannel(7)-500);
                    ui->fc8->setValue(pkgInNew.getChannel(8)-500);
                    ui->fc9->setValue(pkgInNew.getChannel(9));
                    ui->fc0->setValue(pkgInNew.getChannel(10));
                    ui->fc11->setValue(pkgInNew.getChannel(11)-500);
                    ui->fc12->setValue(pkgInNew.getChannel(12)-500);
                    ui->fc13->setValue(pkgInNew.getChannel(13)-500);
                    ui->fc14->setValue(pkgInNew.getChannel(14)-500);
                    ui->fc15->setValue(pkgInNew.getChannel(15)-500);
                break;
                case 38: // Flight-Computer
                    ui->log->append("New Package from Flight-Computer");
                    ui->fcp0->setValue(pkgInNew.getChannel(0));
                    ui->fcp1->setValue(pkgInNew.getChannel(1)-180);
                    ui->fcp2->setValue(pkgInNew.getChannel(2)-180);
                    ui->fcp3->setValue(pkgInNew.getChannel(3));
                    break;
                case 56: // Taranis
                    ui->log->append("New Package from Taranis");
                    ui->ta0->setValue(pkgInNew.getChannel(0));
                    ui->ta1->setValue(pkgInNew.getChannel(1));
                    ui->ta2->setValue(pkgInNew.getChannel(2));
                    ui->ta3->setValue(pkgInNew.getChannel(3));
                    ui->ta4->setValue(pkgInNew.getChannel(4));
                    ui->ta5->setValue(pkgInNew.getChannel(5));
                    ui->ta6->setValue(pkgInNew.getChannel(6));
                    ui->ta7->setValue(pkgInNew.getChannel(7));
                    ui->ta8->setValue(pkgInNew.getChannel(8));
                    ui->ta9->setValue(pkgInNew.getChannel(9));
                    ui->ta0->setValue(pkgInNew.getChannel(10));
                    ui->ta11->setValue(pkgInNew.getChannel(11));
                    ui->ta12->setValue(pkgInNew.getChannel(12));
                    ui->ta13->setValue(pkgInNew.getChannel(13));
                    ui->ta14->setValue(pkgInNew.getChannel(14));
                    ui->ta15->setValue(pkgInNew.getChannel(15));
                    break;
                case 74: // PDB
                    ui->log->append("New Package from PDB");
                    ui->pdb0->setValue(pkgInNew.getChannel(0));
                    ui->pdb1->setValue(pkgInNew.getChannel(1)*128);
                    ui->pdb2->setValue(pkgInNew.getChannel(2)*256);
                    ui->pdb3->setValue(pkgInNew.getChannel(3)*32);
                    ui->pdb4->setValue(pkgInNew.getChannel(4)*64);
                    ui->pdb5->setValue(pkgInNew.getChannel(5));
                    ui->pdb6->setValue(pkgInNew.getChannel(6));
                    ui->pdb7->setValue(pkgInNew.getChannel(7));
                    break;
                default:
                    ui->log->append("Package from unknown transmitter with ID: " + QString::number(pkgInNew.getDeviceId()));
                    break;
            }
        }
    }
}
