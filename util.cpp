#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>

#include "util.h"
#include "mainwindow.h"
#include "modifiedfilesdialog.h"
#include "objects/qsifile.h"


int handleModifiedFiles() {
	/*
	 * if(numOpenFiles == 0)
	 *		return 1;
	 */

	ModifiedFilesDialog m;
	m.exec();
	int clickResult = m.getResult();
	if(clickResult == 2) {
		// user clicked "Save All"
		/*
		 * for(openFille in openFiles) {
		 *		openFile.save();
		 * }
		 */
	}

	return clickResult;
}

QString getWidgetType(QObject* widget) {
	// assume that we're smart enough to already ensure that widget is in fact a widget
	return QString(widget->metaObject()->className());
}

void syntaxHighlighting(QWidget widget, bool yn) {
	// if true, applies. If falls, clears
	if(yn) {
		// just temporary so the compiler doesn't bitch about yn being unreferenced
	}

}

void setTheme(QString theme) {
	QFile styleFile("stylesheet.qss");
	if(!styleFile.open(QFile::ReadOnly)) {
		MainWindow::instance()->console("Failed to open stylesheet: " + styleFile.errorString(), 1);
		return;
	}
	QString stylesheet = QLatin1String(styleFile.readAll());
	styleFile.close();
	qApp->setStyleSheet(stylesheet);
}

bool handleEvents(QEvent e) {

	return true;
}
