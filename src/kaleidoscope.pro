#-------------------------------------------------
#
# Project created by QtCreator 2015-06-03T19:40:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kaleidoscope
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    paintwidget.cpp \
    graph.cpp \
    py.cpp \
    plugins.cpp \
    newnodedialog.cpp

HEADERS  += mainwindow.h \
    paintwidget.h \
    graph.h \
    py.h \
    plugins.h \
    newnodedialog.h

FORMS    += mainwindow.ui \
    newnodedialog.ui

win32 {
    INCLUDEPATH += C:\Python27\include
    LIBS += -LC:\Python27\libs -lpython27
}
unix {
    INCLUDEPATH += /usr/include/python2.7
    LIBS += -lpython2.7
}
