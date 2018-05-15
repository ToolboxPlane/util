#-------------------------------------------------
#
# Project created by QtCreator 2018-04-04T23:38:36
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SerialRcLibViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    rcLib.cpp

HEADERS  += mainwindow.h \
    rcLib.hpp

FORMS    += mainwindow.ui
