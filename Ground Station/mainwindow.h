#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>

#include "plot.h"
#include "RadioControlProtocolCpp/rcLib.hpp"

#define BUF_SIZE 512

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

    void on_buttonClear_clicked();

    void on_buttonPlus_clicked();

    void on_buttonMinus_clicked();

    void on_buttonReset_clicked();

protected slots:
    void readSocket();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *sceneRoll, *scenePitch, *sceneCompass, *sceneMap;
    PlotWidget *rollPlot, *pitchPlot, *yawPlot, *accXPlot, *accYPlot, *accZPlot, *altPlot, *altGndPlot, *speedPlot;
    QTimer *timer{nullptr};
    uint8_t buf[BUF_SIZE];
    double lastX, lastY;
    void handlePackage(rcLib::Package pkgInNew);
    int fd{0};
};

#endif // MAINWINDOW_H
