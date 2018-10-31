QT += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = QtSphereIDE
TEMPLATE = app
VERSION = 0.5.0
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
	settingswindow.cpp \
	formats/mapfile.cpp \
	formats/spherefile.cpp \
	qsiproject.cpp \
	projectpropertiesdialog.cpp \
	formats/spriteset.cpp \
	editors/spriteseteditor.cpp \
	palettewidget.cpp \
	imagechooser.cpp \
	drawingwidget.cpp \
	editors/ssdirectionview.cpp \
	soundplayer.cpp \
	importoptionsdialog.cpp \
	startpage.cpp \
	editors/sphereeditor.cpp \
	editors/texteditor.cpp \
	editors/mapeditor.cpp

HEADERS += \
	mainwindow.h \
	aboutdialog.h \
	util.h \
	modifiedfilesdialog.h \
	settingswindow.h \
	formats/mapfile.h \
	formats/spherefile.h \
	qsiproject.h \
	projectpropertiesdialog.h \
	formats/spriteset.h \
    editors/spriteseteditor.h \
	palettewidget.h \
	imagechooser.h \
	drawingwidget.h \
	editors/ssdirectionview.h \
	soundplayer.h \
	importoptionsdialog.h \
	startpage.h \
	editors/sphereeditor.h \
	editors/texteditor.h \
	editors/mapeditor.h

FORMS += \
	mainwindow.ui \
	aboutdialog.ui \
	modifiedfilesdialog.ui \
	settingswindow.ui \
	projectpropertiesdialog.ui \
	editors/spriteseteditor.ui \
	drawingwidget.ui \
	soundplayer.ui \
	importoptionsdialog.ui \
	startpage.ui \
	editors/mapeditor.ui

RESOURCES += res/qsires.qrc

RC_FILE = res/icon.rc

DISTFILES +=
