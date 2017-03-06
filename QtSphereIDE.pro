QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtSphereIDE
TEMPLATE = app
VERSION = 0.2
DEFINES += QT_DEPRECATED_WARNINGS TARGET=\\\"$$TARGET\\\" VERSION=\\\"$$VERSION\\\"


SOURCES += main.cpp\
    mainwindow.cpp \
    aboutdialog.cpp \
    util.cpp \
    modifiedfilesdialog.cpp \
    config.cpp \
    texteffects.cpp \
    settingswindow.cpp \
    objects/map.cpp \
    objects/tileset.cpp \
    objects/qsifile.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    util.h \
    modifiedfilesdialog.h \
    config.h \
    texteffects.h \
    settingswindow.h \
    objects/map.h \
    objects/tileset.h \
    objects/qsifile.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    modifiedfilesdialog.ui \
    settingswindow.ui

RESOURCES += \
    res/qsires.qrc

#QMAKE_CXXFLAGS += /utf-8


RC_FILE = res/icon.rc
