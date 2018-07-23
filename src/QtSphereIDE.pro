QT += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = QtSphereIDE
TEMPLATE = app
VERSION = 0.3.0
DEFINES += QT_DEPRECATED_WARNINGS TARGET=\\\"$$TARGET\\\" VERSION=\\\"$$VERSION\\\"
QMAKE_LFLAGS += -no-pie

UI_DIR=temp_files
DESTDIR=build
OBJECTS_DIR=temp_files
MOC_DIR=temp_files

SOURCES += \
	main.cpp\
	mainwindow.cpp \
	aboutdialog.cpp \
	util.cpp \
	modifiedfilesdialog.cpp \
	config.cpp \
	settingswindow.cpp \
	objects/mapfile.cpp \
	objects/spherefile.cpp \
	qsiproject.cpp \
	projectpropertiesdialog.cpp \
	objects/spriteset.cpp \
	spritesetview.cpp \
	palettewidget.cpp \
	imagechooser.cpp \
	drawingwidget.cpp \
	ssdirectionview.cpp \
	soundplayer.cpp \
	importoptionsdialog.cpp

HEADERS += \
	mainwindow.h \
	aboutdialog.h \
	util.h \
	modifiedfilesdialog.h \
	config.h \
	settingswindow.h \
	objects/mapfile.h \
	objects/spherefile.h \
	qsiproject.h \
	projectpropertiesdialog.h \
	objects/spriteset.h \
	spritesetview.h \
	palettewidget.h \
	imagechooser.h \
	drawingwidget.h \
	ssdirectionview.h \
	soundplayer.h \
	importoptionsdialog.h

FORMS += \
	mainwindow.ui \
	aboutdialog.ui \
	modifiedfilesdialog.ui \
	settingswindow.ui \
	projectpropertiesdialog.ui \
	spritesetview.ui \
	drawingwidget.ui \
	soundplayer.ui \
	importoptionsdialog.ui

RESOURCES += res/qsires.qrc

RC_FILE = res/icon.rc

DISTFILES +=
