/**
 * @file MainWindow.cpp
 * @author paul
 * @date 24.11.22
 * Description here TODO
 */
#include "MainWindow.hpp"

namespace ground_station {
    MainWindow::MainWindow(QWidget *parent) : QWidget{parent} {
        this->setLayout(&mainLayout);

        mainLayout.addLayout(&connectionWidget);
        mainLayout.addWidget(&tabWidget);
        mainLayout.addWidget(&loggingWidget);

        tabWidget.addTab(&fcWidget, "FC");

        loggingWidget.logError("Error");
        loggingWidget.logInfo("Info");
        loggingWidget.logWarn("Warn");

        QObject::connect(&controller, &Controller::fcDataReceived, &fcWidget, &FcWidget::fcDataReceived);

        controller.connectToFc();
    }
} // namespace ground_station
