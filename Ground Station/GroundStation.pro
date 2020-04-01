#-------------------------------------------------
#
# Project created by QtCreator 2018-04-04T23:38:36
#
#-------------------------------------------------

QT       += core gui serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GroundStation
TEMPLATE = app

CONFIG += c++17

SOURCES += main.cpp\
        mainwindow.cpp \
        plot.cpp \
        RadioControlProtocolCpp/rcLib.cpp \
        RadioControlProtocolCpp/RadioControlProtocolC/rc_lib.c

HEADERS  += mainwindow.h \
        plot.h \
        RadioControlProtocolCpp/RadioControlProtocolC/rc_lib.h \
        RadioControlProtocolCpp/rcLib.hpp

FORMS    += mainwindow.ui
