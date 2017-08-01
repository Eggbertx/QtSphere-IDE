QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtSphereIDE
TEMPLATE = app
VERSION = 0.2.3
DEFINES += QT_DEPRECATED_WARNINGS TARGET=\\\"$$TARGET\\\" VERSION=\\\"$$VERSION\\\"

UI_DIR=temp_files
DESTDIR=build
OBJECTS_DIR=temp_files
MOC_DIR=temp_files

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

RC_FILE = res/icon.rc
