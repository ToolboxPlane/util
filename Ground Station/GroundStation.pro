#-------------------------------------------------
#
# Project created by QtCreator 2018-04-04T23:38:36
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GroundStation
TEMPLATE = app

CONFIG += c++14

SOURCES += main.cpp\
        mainwindow.cpp \
    rcLib.cpp \
    plot.cpp

HEADERS  += mainwindow.h \
    rcLib.hpp \
    plot.h

FORMS    += mainwindow.ui
