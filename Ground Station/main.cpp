#include <QApplication>

#include "Widgets/MainWindow.hpp"
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ground_station::MainWindow mainWindow;
    mainWindow.show();
    return a.exec();
}
