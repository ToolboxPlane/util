#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>

#include "plot.h"
#include "rcLib.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();

    void on_spinBox_valueChanged(int arg1);

protected slots:
    void readSocket();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *sceneRoll, *scenePitch, *sceneCompass;
    Plot *rollPlot, *pitchPlot;
    QTimer *timer{nullptr};
    uint8_t buf[512];
    void handlePackage(rcLib::Package pkgInNew, int transmitterId = -1);
    int fd{0};
};

#endif // MAINWINDOW_H
