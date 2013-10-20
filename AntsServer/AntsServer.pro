#-------------------------------------------------
#
# Project created by QtCreator 2013-10-13T21:29:15
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AntsServer
TEMPLATE = app


SOURCES += main.cpp\
        map.cpp \
    server.cpp \
    client.cpp

HEADERS  += map.h \
    server.h \
    client.h \
    constants.h

FORMS    += map.ui
