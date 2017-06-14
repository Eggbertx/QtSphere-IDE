QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtSphereIDE
TEMPLATE = app
VERSION = 0.2.2
DEFINES += QT_DEPRECATED_WARNINGS TARGET=\\\"$$TARGET\\\" VERSION=\\\"$$VERSION\\\"
CONFIG += static
INCLUDEPATH += $$PWD/include


win32:LIBS += -L$$PWD/lib -lqscintilla2
unix:LIBS += -lqscintilla2_qt5


SOURCES += main.cpp\
    mainwindow.cpp \
    aboutdialog.cpp \
    util.cpp \
    modifiedfilesdialog.cpp \
    config.cpp \
    settingswindow.cpp \
    objects/qsifile.cpp \
    objects/mapfile.cpp \
    objects/textfile.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    util.h \
    modifiedfilesdialog.h \
    config.h \
    settingswindow.h \
    objects/qsifile.h \
    objects/mapfile.h \
    objects/textfile.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    modifiedfilesdialog.ui \
    settingswindow.ui

RESOURCES += \
    res/qsires.qrc

#QMAKE_CXXFLAGS += /utf-8


RC_FILE = res/icon.rc
