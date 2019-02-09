QT += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = QtSphereIDE
TEMPLATE = app
VERSION = 0.8.1
DEFINES += QT_DEPRECATED_WARNINGS TARGET=\\\"$$TARGET\\\" VERSION=\\\"$$VERSION\\\"
QMAKE_LFLAGS += -no-pie

UI_DIR=temp_files
DESTDIR=build
OBJECTS_DIR=temp_files
MOC_DIR=temp_files

SOURCES += \
	main.cpp\
	mainwindow.cpp \
	dialogs/aboutdialog.cpp \
	util.cpp \
	dialogs/modifiedfilesdialog.cpp \
	dialogs/settingswindow.cpp \
	formats/mapfile.cpp \
	formats/spherefile.cpp \
	qsiproject.cpp \
	dialogs/projectpropertiesdialog.cpp \
	formats/spriteset.cpp \
	widgets/spriteset/spriteseteditor.cpp \
	widgets/palettewidget.cpp \
	widgets/imagechooser.cpp \
	widgets/drawingwidget.cpp \
	widgets/spriteset/ssdirectionview.cpp \
	widgets/soundplayer.cpp \
	dialogs/importoptionsdialog.cpp \
	widgets/startpage.cpp \
	widgets/sphereeditor.cpp \
	widgets/texteditor.cpp \
	widgets/map/mapeditor.cpp \
	formats/tileset.cpp \
	widgets/wrappedgraphicsview.cpp \
	widgets/map/mapview.cpp \
	widgets/colorbutton.cpp \
	dialogs/entityeditdialog.cpp \
	dialogs/layerpropertiesdialog.cpp

HEADERS += \
	mainwindow.h \
	dialogs/aboutdialog.h \
	util.h \
	dialogs/modifiedfilesdialog.h \
	dialogs/settingswindow.h \
	formats/mapfile.h \
	formats/spherefile.h \
	qsiproject.h \
	dialogs/projectpropertiesdialog.h \
	formats/spriteset.h \
	widgets/spriteset/spriteseteditor.h \
	widgets/palettewidget.h \
	widgets/imagechooser.h \
	widgets/drawingwidget.h \
	widgets/spriteset/ssdirectionview.h \
	widgets/soundplayer.h \
	dialogs/importoptionsdialog.h \
	widgets/startpage.h \
	widgets/sphereeditor.h \
	widgets/texteditor.h \
	widgets/map/mapeditor.h \
	formats/tileset.h \
	widgets/wrappedgraphicsview.h \
	widgets/map/mapview.h \
	widgets/colorbutton.h \
	dialogs/entityeditdialog.h \
	dialogs/layerpropertiesdialog.h

FORMS += \
	mainwindow.ui \
	dialogs/aboutdialog.ui \
	dialogs/modifiedfilesdialog.ui \
	dialogs/settingswindow.ui \
	dialogs/projectpropertiesdialog.ui \
	widgets/spriteset/spriteseteditor.ui \
	widgets/drawingwidget.ui \
	widgets/soundplayer.ui \
	dialogs/importoptionsdialog.ui \
	widgets/startpage.ui \
	widgets/map/mapeditor.ui \
	dialogs/entityeditdialog.ui \
	dialogs/layerpropertiesdialog.ui

RESOURCES += res/qsires.qrc

RC_FILE = res/icon.rc

DISTFILES +=
