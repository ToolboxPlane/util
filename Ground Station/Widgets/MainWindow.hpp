/**
 * @file MainWindow.hpp
 * @author paul
 * @date 24.11.22
 * Description here TODO
 */
#ifndef GROUNDSTATION_MAINWINDOW_HPP
#define GROUNDSTATION_MAINWINDOW_HPP

#include <QVBoxLayout>
#include <QTabWidget>
#include <QWidget>

#include "ConnectionWidget.hpp"
#include "LoggingWidget.hpp"
#include "FcWidget.hpp"
#include "../Controller.hpp"

namespace ground_station {

    class MainWindow : public QWidget {
        Q_OBJECT

      public:
        explicit MainWindow(QWidget *parent = nullptr);

      private slots:

      private:
        Controller controller;

        QVBoxLayout mainLayout;
        ConnectionWidget connectionWidget;
        QTabWidget tabWidget;
        FcWidget fcWidget;
        LoggingWidget loggingWidget;
    };

} // namespace ground_station

#endif // GROUNDSTATION_MAINWINDOW_HPP
