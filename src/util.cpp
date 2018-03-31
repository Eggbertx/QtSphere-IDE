#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>

#include "util.h"
#include "mainwindow.h"
#include "modifiedfilesdialog.h"
#include "objects/spherefile.h"


int handleModifiedFiles() {
	// if(numOpenFiles == 0) return 1;

	ModifiedFilesDialog m;
	m.exec();
	int clickResult = m.getResult();
	if(clickResult == 2) {
		// user clicked "Save All"
		/* for(openFille in openFiles) {
			openFile.save();
		} */
	}
	return clickResult;
}

QString getWidgetType(QObject* widget) {
	// assume that we're smart enough to already ensure that widget is in fact a widget
	return QString(widget->metaObject()->className());
}

bool handleEvents(QEvent e) {

	return true;
}

void errorBox(QString message) {
	QMessageBox::critical(MainWindow::instance(), "Error!", message, QMessageBox::Ok);
}

void infoBox(QString info) {
	QMessageBox::information(MainWindow::instance(), "Info", info, QMessageBox::Ok);
}
