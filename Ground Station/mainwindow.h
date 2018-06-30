#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QFile>
#include <QTextStream>
#include <QThread>

#include "csvreaderthread.h"
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

    void on_buttonOpenFile_clicked();

protected slots:
    void serialRead();
    void forwardResult(QStringList items);

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QGraphicsScene *sceneRoll, *scenePitch, *sceneCompass;
    Plot *rollPlot, *pitchPlot;
    void handlePackage(rcLib::Package pkgInNew, int transmitterId = -1);
    QFile *recordingFile;
    CsvReaderThread *csvThread;
};

#endif // MAINWINDOW_H
