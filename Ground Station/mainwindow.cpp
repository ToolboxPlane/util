#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <cmath>

#include <QDebug>
#include <QFile>

constexpr auto WAYPOINT_SIZE = 10;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFile f(":qdarkstyle/style.qss");

    if (!f.exists())   {
        qDebug() << "Unable to set stylesheet, file not found\n";
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }


    sceneRoll = new QGraphicsScene(this);
    scenePitch = new QGraphicsScene(this);
    sceneCompass = new QGraphicsScene(this);
    sceneMap = new QGraphicsScene(this);

    ui->viewRoll->setScene(sceneRoll);
    ui->viewPitch->setScene(scenePitch);
    ui->viewCompass->setScene(sceneCompass);
    ui->viewMap->setScene(sceneMap);

    sceneRoll->setBackgroundBrush(QBrush(Qt::black));
    scenePitch->setBackgroundBrush(QBrush(Qt::black));
    sceneCompass->setBackgroundBrush(QBrush(Qt::black));
    sceneMap->setBackgroundBrush(QBrush(Qt::black));

    sceneRoll->addEllipse(80, -8, 40, 16, QPen(Qt::darkBlue), QBrush(Qt::lightGray));
    sceneRoll->addRect(0, 0, 200, 5, QPen(Qt::darkBlue), QBrush(Qt::lightGray));
    sceneRoll->addRect(66, 0, 5, -20, QPen(Qt::darkBlue), QBrush(Qt::lightGray));
    sceneRoll->addRect(200-(66+5), 0, 5, -20, QPen(Qt::darkBlue), QBrush(Qt::lightGray));

    sceneRoll->addLine(100,0, 100+50, 0, QPen(Qt::green, 4));
    sceneRoll->addLine(100,0, 100, 50, QPen(Qt::blue, 4));
    sceneRoll->addLine(100-3,-3,100+3,3, QPen(Qt::red, 4));
    sceneRoll->addLine(100-3,3,100+3,-3, QPen(Qt::red, 4));

    ui->viewRoll->scale(3, 3);

    scenePitch->addEllipse(0, 0, 70, 16, QPen(Qt::darkBlue), QBrush(Qt::lightGray));
    scenePitch->addRect(75,-10,-30,20, QPen(Qt::darkBlue), QBrush(Qt::lightGray));
    scenePitch->addRect(0,5,80,5, QPen(Qt::darkBlue), QBrush(Qt::lightGray));

    ui->viewPitch->scale(4, 4);

    scenePitch->addLine(40, 7.5, 40-50, 7.5, QPen(Qt::red, 4));
    scenePitch->addLine(40, 7.5, 40, 7.5+50, QPen(Qt::blue, 4));
    scenePitch->addLine(40-3, 7.5-3, 40+3, 7.5+3, QPen(Qt::green, 4));
    scenePitch->addLine(40-3, 7.5+3, 40+3, 7.5-3, QPen(Qt::green, 4));

    sceneCompass->addRect(-5,-50,10,100, QPen(Qt::black), QBrush(Qt::red));
    sceneCompass->addRect(-5,50,10,100, QPen(Qt::black), QBrush(Qt::white));

    sceneMap->addEllipse(-WAYPOINT_SIZE/2, -WAYPOINT_SIZE/2, WAYPOINT_SIZE, WAYPOINT_SIZE, QPen(Qt::blue), QBrush(Qt::darkGray));
    lastX = lastY = 0;

    rollPlot = new PlotWidget();
    pitchPlot = new PlotWidget();
    yawPlot = new PlotWidget();
    accXPlot = new PlotWidget();
    accYPlot = new PlotWidget();
    accZPlot = new PlotWidget();
    altPlot = new PlotWidget();
    altGndPlot = new PlotWidget();
    speedPlot = new PlotWidget();

    ui->gridLayout->addWidget(rollPlot, 1,0);
    ui->gridLayout->addWidget(pitchPlot, 1, 2);
    ui->gridLayout->addWidget(yawPlot, 1, 4);
    ui->gridLayout->addWidget(accXPlot, 4,0);
    ui->gridLayout->addWidget(accYPlot, 4, 2);
    ui->gridLayout->addWidget(accZPlot, 4, 4);
    ui->gridLayout->addWidget(altGndPlot, 7, 0);
    ui->gridLayout->addWidget(altPlot, 7, 2);
    ui->gridLayout->addWidget(speedPlot, 7, 4);

    ui->viewRoll->rotate(20);
    ui->viewPitch->rotate(-20);
    ui->viewCompass->rotate(45);

    fd = socket(AF_INET, SOCK_RAW | SOCK_NONBLOCK, 253);
    if (fd < 0) {
        qDebug() << strerror(errno);
        ui->log->append(QString("Error establishing connection: ") + QString(strerror(errno)));

    } else {
        if (!timer) {
            timer = new QTimer();
            connect(timer, SIGNAL(timeout()), this, SLOT(readSocket()));
            timer->start(100);
        }
    }
}

MainWindow::~MainWindow()
{
    delete sceneRoll;
    delete scenePitch;
    delete sceneCompass;
    delete timer;
    delete rollPlot;
    delete pitchPlot;
    delete yawPlot;
    delete accXPlot;
    delete accYPlot;
    delete accZPlot;
    delete altPlot;
    delete altGndPlot;
    delete speedPlot;
}

void MainWindow::readSocket()
{
    static rcLib::Package pkgInNew;
    auto readed = recv(this->fd, this->buf, BUF_SIZE, 0);
    if (readed > 0) {
        auto *ip_packet = reinterpret_cast<iphdr*>(this->buf);
        for (auto c = ip_packet->ihl*4; c < readed; c++) {
            if (pkgInNew.decode(this->buf[c])) {
                this->handlePackage(pkgInNew);
            }
        }
    } /*else {
        qDebug() << strerror(errno);
    }*/
}

void MainWindow::handlePackage(rcLib::Package pkgInNew)
{
    int transmitterId = pkgInNew.getDeviceId();
    switch (transmitterId) {
        case 17: // lora-remote
            ui->log->append("New Package from LoRa-Remote");
            ui->lora0->setValue(pkgInNew.getChannel(0)-127);
            ui->lora1->setValue(pkgInNew.getChannel(1)-127);
            ui->lora2->setValue(pkgInNew.getChannel(2)-127);
            ui->lora3->setValue(pkgInNew.getChannel(3)-127);
            ui->lora4->setValue(pkgInNew.getChannel(4));
            ui->lora5->setValue(pkgInNew.getChannel(5));
            ui->lora6->setValue(pkgInNew.getChannel(6));
            ui->lora7->setValue(-pkgInNew.getChannel(7));

            ui->joyRightX->setValue(pkgInNew.getChannel(0)-127);
            ui->joyRightY->setValue(pkgInNew.getChannel(1)-127);
            ui->joyLeftX->setValue(pkgInNew.getChannel(2)-127);
            ui->joyLeftY->setValue(pkgInNew.getChannel(3)-127);

        break;
        case 23: // Flight-Controller
            ui->log->append("New Package from Flight-Controller");
            ui->fc0->setValue(pkgInNew.getChannel(0));
            ui->fc1->setValue((pkgInNew.getChannel(1) - 500) / 2.0);
            ui->fc2->setValue((pkgInNew.getChannel(2) - 500) / 2.0);
            ui->fc3->setValue((pkgInNew.getChannel(3) - 500) / 2.0);
            ui->fc4->setValue((pkgInNew.getChannel(4) - 500) / 16.0);
            ui->fc5->setValue((pkgInNew.getChannel(5) - 500) / 16.0);
            ui->fc6->setValue((pkgInNew.getChannel(6) - 500) / 16.0);
            ui->fc7->setValue((pkgInNew.getChannel(7) - 500) / 6.25);
            ui->fc8->setValue((pkgInNew.getChannel(8) - 500) / 6.25);
            ui->fc9->setValue((pkgInNew.getChannel(9) - 500) / 6.25);
            ui->fc10->setValue(pkgInNew.getChannel(10));
            ui->fc11->setValue(pkgInNew.getChannel(11));
            ui->fc12->setValue(pkgInNew.getChannel(12));
            ui->fc13->setValue(pkgInNew.getChannel(13));
            ui->fc14->setValue(pkgInNew.getChannel(14)-500);
            ui->fc15->setValue(pkgInNew.getChannel(15)-500);

            rollPlot->addValue((pkgInNew.getChannel(1)-500)/2.0, 1);
            pitchPlot->addValue((pkgInNew.getChannel(2)-500)/2.0, 1);
            yawPlot->addValue((pkgInNew.getChannel(3)-500)/2.0, 1);
            accXPlot->addValue((pkgInNew.getChannel(7)-500)/6.25, 1);
            accYPlot->addValue((pkgInNew.getChannel(8)-500)/6.25, 1);
            accZPlot->addValue((pkgInNew.getChannel(9)-500)/6.25, 1);

            ui->motor->setValue(pkgInNew.getChannel(13));
            ui->vtailLeft->setValue(pkgInNew.getChannel(14)-500);
            ui->vtailRight->setValue(pkgInNew.getChannel(15)-500);
        break;
        case 38: // Flight-Computer
            ui->log->append("New Package from Flight-Computer");
            ui->fcp0->setValue(-pkgInNew.getChannel(0));
            ui->fcp1->setValue(pkgInNew.getChannel(1) / 10.0);
            ui->fcp2->setValue((pkgInNew.getChannel(2)-500) / 2.0);
            ui->fcp3->setValue(pkgInNew.getChannel(3) * 10.0);
            ui->fcp4->setValue((pkgInNew.getChannel(4)-500)/2.0);
            ui->fcp5->setValue((pkgInNew.getChannel(5)-500)/2.0);
            ui->fcp6->setValue((pkgInNew.getChannel(6)-500)/2.0);
            ui->fcp7->setValue(pkgInNew.getChannel(7) / 10.0);
            ui->fcp8->setValue(pkgInNew.getChannel(8));
            ui->fcp9->setValue((pkgInNew.getChannel(9)-500) / 10.0);
            ui->fcp10->setValue((pkgInNew.getChannel(10)-500) / 10.0);
            ui->fcp11->setValue((pkgInNew.getChannel(11)-500) / 10.0);
            ui->fcp12->setValue(pkgInNew.getChannel(12) / 50.0);
            ui->fcp13->setValue(pkgInNew.getChannel(13));
            ui->fcp14->setValue(pkgInNew.getChannel(14));
            ui->fcp15->setValue(pkgInNew.getChannel(15));

            ui->viewCompass->resetTransform();
            ui->viewPitch->resetTransform();
            ui->viewRoll->resetTransform();
            ui->viewCompass->resetTransform();
            ui->viewCompass->rotate((pkgInNew.getChannel(6)-500)/2.0);
            ui->viewRoll->rotate((pkgInNew.getChannel(4)-500)/2.0);
            ui->viewPitch->rotate((pkgInNew.getChannel(5)-500)/2.0);
            ui->viewRoll->scale(3, 3);
            ui->viewPitch->scale(4, 4);
            ui->viewCompass->scale(0.9, 0.9);

            {
                auto alpha = -((pkgInNew.getChannel(2)-500) / 2.0) - 90;
                auto dist = pkgInNew.getChannel(3) * 10.0;
                auto x = std::cos(alpha/180 * M_PI) * dist;
                auto y = -std::sin(alpha/180 * M_PI) * dist;

                sceneMap->addLine(lastX, lastY, x, y, QPen(Qt::blue));
                sceneMap->addEllipse(lastX-WAYPOINT_SIZE/2, lastY-WAYPOINT_SIZE/2, WAYPOINT_SIZE, WAYPOINT_SIZE, QPen(Qt::blue), QBrush(Qt::darkGray));
                sceneMap->addEllipse(x-WAYPOINT_SIZE/2, y-WAYPOINT_SIZE/2, WAYPOINT_SIZE, WAYPOINT_SIZE, QPen(Qt::blue), QBrush(Qt::darkGray));
                lastX = x;
                lastY = y;
            }


            rollPlot->addValue((pkgInNew.getChannel(4)-500)/2.0, 0);
            pitchPlot->addValue((pkgInNew.getChannel(5)-500)/2.0, 0);
            yawPlot->addValue((pkgInNew.getChannel(6)-500)/2.0, 0);
            accXPlot->addValue((pkgInNew.getChannel(9)-500)/10.0, 0);
            accYPlot->addValue((pkgInNew.getChannel(10)-500)/10.0, 0);
            accZPlot->addValue((pkgInNew.getChannel(11)-500)/10.0, 0);
            altPlot->addValue(pkgInNew.getChannel(8), 0);
            altGndPlot->addValue(pkgInNew.getChannel(1) / 10.0, 0);
            speedPlot->addValue(pkgInNew.getChannel(7) / 10.0, 0);

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
            ui->ta10->setValue(pkgInNew.getChannel(10));
            ui->ta11->setValue(pkgInNew.getChannel(11));
            ui->ta12->setValue(pkgInNew.getChannel(12));
            ui->ta13->setValue(pkgInNew.getChannel(13));
            ui->ta14->setValue(pkgInNew.getChannel(14));
            ui->ta15->setValue(pkgInNew.getChannel(15));
            break;
        case 74: // PDB
            ui->log->append("New Package from PDB");
            ui->pdb0->setValue((pkgInNew.getChannel(0)*128) / 1000.0);
            ui->pdb1->setValue((pkgInNew.getChannel(1)*256) / 1000.0);
            ui->pdb2->setValue((pkgInNew.getChannel(2)*32) / 1000.0);
            ui->pdb3->setValue((pkgInNew.getChannel(3)*64) / 1000.0);
            break;
        case 91: // Nav-Board
            ui->log->append("New Package from Nav");
            ui->nav0->setValue(-pkgInNew.getChannel(0));
            ui->nav1->setValue(pkgInNew.getChannel(1));
            ui->nav2->setValue(pkgInNew.getChannel(2));
            ui->nav3->setValue(pkgInNew.getChannel(3) / 100.0);
            break;
        default:
            ui->log->append("Package from unknown transmitter with ID: " + QString::number(pkgInNew.getDeviceId()));
            break;
    }
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    pitchPlot->setXSteps(arg1);
    rollPlot->setXSteps(arg1);
    yawPlot->setXSteps(arg1);
    accXPlot->setXSteps(arg1);
    accYPlot->setXSteps(arg1);
    accZPlot->setXSteps(arg1);
    altPlot->setXSteps(arg1);
    altGndPlot->setXSteps(arg1);
    speedPlot->setXSteps(arg1);
    update();
}

void MainWindow::on_buttonClear_clicked()
{
    pitchPlot->clear();
    rollPlot->clear();
    yawPlot->clear();
    accXPlot->clear();
    accYPlot->clear();
    accZPlot->clear();
    altPlot->clear();
    altGndPlot->clear();
    speedPlot->clear();
    update();
}

void MainWindow::on_buttonPlus_clicked()
{
    ui->viewMap->scale(2, 2);
}

void MainWindow::on_buttonMinus_clicked()
{
    ui->viewMap->scale(0.5, 0.5);
}

void MainWindow::on_buttonReset_clicked()
{
    ui->viewMap->resetTransform();
}
