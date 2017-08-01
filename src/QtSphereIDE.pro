QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtSphereIDE
TEMPLATE = app
VERSION = 0.2.3
DEFINES += QT_DEPRECATED_WARNINGS TARGET=\\\"$$TARGET\\\" VERSION=\\\"$$VERSION\\\"
INCLUDEPATH += $$PWD/include

SOURCES += main.cpp\
    mainwindow.cpp \
    aboutdialog.cpp \
    util.cpp \
    modifiedfilesdialog.cpp \
    config.cpp \
    settingswindow.cpp \
    syntaxhighlighter.cpp \
    objects/mapfile.cpp \
    objects/spherefile.cpp \
    qsiproject.cpp \
    projectpropertiesdialog.cpp
    

HEADERS  += mainwindow.h \
    aboutdialog.h \
    util.h \
    modifiedfilesdialog.h \
    config.h \
    settingswindow.h \
    syntaxhighlighter.h \
    objects/mapfile.h \
    objects/spherefile.h \
    qsiproject.h \
    projectpropertiesdialog.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    modifiedfilesdialog.ui \
    settingswindow.ui \
    projectpropertiesdialog.ui

RESOURCES += \
    res/qsires.qrc

#QMAKE_CXXFLAGS += /utf-8


RC_FILE = res/icon.rc
