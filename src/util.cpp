#include <fstream>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>
#include <QDataStream>
#include <QFile>
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
	if(!widget->isWidgetType()) return "";
	return QString(widget->metaObject()->className());
}

void errorBox(QString message) {
	QMessageBox::critical(MainWindow::instance(), "Error!", message, QMessageBox::Ok);
}

void infoBox(QString info) {
	QMessageBox::information(MainWindow::instance(), "Info", info, QMessageBox::Ok);
}

bool readFile(QString filename, void* into, int size, int offset) {
	if(size == 0) return true;
	std::ifstream is(filename.toStdString().c_str(),  std::ifstream::binary);
	if(!is.is_open()) {
		qDebug().noquote() << "Error loading" << filename;
		return false;
	}
	if(offset > 0) is.ignore(offset);
	is.read((char *)into,size);
	is.close();

	return true;
}
