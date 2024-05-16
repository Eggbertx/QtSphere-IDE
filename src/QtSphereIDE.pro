QT += core gui widgets multimedia xml widgets

TARGET = QtSphereIDE
TEMPLATE = app
VERSION = 0.9
DEFINES += QT_DEPRECATED_WARNINGS TARGET=\\\"$$TARGET\\\" VERSION=\\\"$$VERSION\\\"

SOURCES += \
	formats/qsitasklist.cpp \
	formats/spherefont.cpp \
	palette_presets.cpp \
	dialogs/newmapdialog.cpp \
	main.cpp\
	mainwindow.cpp \
	util.cpp \
	dialogs/modifiedfilesdialog.cpp \
	dialogs/settingswindow.cpp \
	formats/mapfile.cpp \
	formats/spherefile.cpp \
	qsiproject.cpp \
	dialogs/projectpropertiesdialog.cpp \
	formats/spriteset.cpp \
	widgets/fonteditor.cpp \
	widgets/maintabbar.cpp \
	widgets/maintabwidget.cpp \
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
	dialogs/layerpropertiesdialog.cpp \
	commands/mapdrawcommand.cpp

HEADERS += \
	formats/qsitasklist.h \
	formats/spherefont.h \
	palette_presets.h \
	dialogs/newmapdialog.h \
	mainwindow.h \
	util.h \
	dialogs/modifiedfilesdialog.h \
	dialogs/settingswindow.h \
	formats/mapfile.h \
	formats/spherefile.h \
	qsiproject.h \
	dialogs/projectpropertiesdialog.h \
	formats/spriteset.h \
	widgets/fonteditor.h \
	widgets/maintabbar.h \
	widgets/maintabwidget.h \
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
	dialogs/layerpropertiesdialog.h \
	commands/mapdrawcommand.h

FORMS += \
	dialogs/newmapdialog.ui \
	mainwindow.ui \
	dialogs/modifiedfilesdialog.ui \
	dialogs/settingswindow.ui \
	dialogs/projectpropertiesdialog.ui \
	widgets/fonteditor.ui \
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
