#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include "plot.h"

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
    void serialRead();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QGraphicsScene *sceneRoll, *scenePitch, *sceneCompass;
    Plot *rollPlot, *pitchPlot;
};

#endif // MAINWINDOW_H
