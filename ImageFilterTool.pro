#-------------------------------------------------
#
# Project created by QtCreator 2017-04-24T17:17:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageFilterTool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imageannotations.cpp \
    imageviewwidget.cpp \
    scrollareanowheel.cpp

HEADERS  += mainwindow.h \
    imageannotations.h \
    imageviewwidget.h \
    scrollareanowheel.h

FORMS    += mainwindow.ui

INCLUDEPATH += C:\Boost\include\boost-1_59

LIBS += C:\Boost\lib\*.lib
