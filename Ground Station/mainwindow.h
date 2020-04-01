#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>

#include "plot.h"
#include "RadioControlProtocolCpp/rcLib.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_spinBox_valueChanged(int arg1);

protected slots:
    void readSocket();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *sceneRoll, *scenePitch, *sceneCompass;
    Plot *rollPlot, *pitchPlot;
    QTimer *timer{nullptr};
    uint8_t buf[512];
    void handlePackage(rcLib::Package pkgInNew);
    int fd{0};
};

#endif // MAINWINDOW_H
