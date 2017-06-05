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

INCLUDEPATH += D:\libs\boost_1_59_0\

LIBS += D:\libs\boost1.59.0-win32-vc120\lib\*.lib # Static 32

#LIBS += D:\libs\boost_1_59_0\lib64-msvc-12.0\*.lib # Static 64

#LIBS += D:\libs\boost1.59.0-x64-vc120\lib\*.lib # Dynamic 64
