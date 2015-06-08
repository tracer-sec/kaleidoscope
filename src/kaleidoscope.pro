#-------------------------------------------------
#
# Project created by QtCreator 2015-06-03T19:40:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kaleidoscope
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    paintwidget.cpp \
    graph.cpp

HEADERS  += mainwindow.h \
    paintwidget.h \
    graph.h

FORMS    += mainwindow.ui
