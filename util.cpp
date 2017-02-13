#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>
#include "mainwindow.h"
#include "util.h"
#include "modifiedfilesdialog.h"


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


void setTheme(QString theme) {
	//QFile styleFile("stylesheet.qss");
	//styleFile.open(QFile::ReadOnly);
	//QString stylesheet = QLatin1String(styleFile.readAll());
	//styleFile.close();
	//qApp->setStyleSheet(stylesheet);

	if(MainWindow::instance() == NULL)
        qDebug() << "MainWindow::instance() is NULL. wtf??";
	else
        qDebug().noquote() << "something something load theme or whatever";
}
