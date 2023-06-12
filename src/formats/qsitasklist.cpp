#include <QFileDialog>

#include "mainwindow.h"
#include "qsitasklist.h"
#include "util.h"

QSITaskList::QSITaskList(QTableWidget* parent) : QObject(parent) {
	m_checks = QList<bool>();
	m_strings = QList<QString>();
	m_widget = parent;
}

void QSITaskList::attach(QTableWidget* widget) {
	if(widget)
		m_widget = widget;
}

void QSITaskList::loadList(QString path) {
	if(path == "") {

	}
}

void QSITaskList::saveList(bool saveAs) {
	int numLines = m_widget->rowCount();
	if(numLines == 0)
		return;
	QString out = "";
	QString path = "";

	QTableWidgetItem* item;
	for(int i = 0; i < numLines; i++) {
		out += "[";
		item = m_widget->item(i, 0);
		out += item->checkState() == Qt::Checked?"x":" ";
		out += "] " + item->text() + "\n";
	}
	if(m_file == nullptr) {
		m_file = new QFile(path);
		saveAs = true;
	}
	path = m_file->fileName();
	if(saveAs)
		path = QFileDialog::getSaveFileName(MainWindow::instance(), "Save tasks", "", "QtSphere IDE task lists (*.txt)");
	if(!path.endsWith(".txt", Qt::CaseInsensitive))
		path += ".txt";
	m_file->setFileName(path);
	writeFile(m_file, out.toLatin1(), out.size());
}

void QSITaskList::setTaskStatus(int index, bool status) {
	if(m_checks.length() < index)
		return;
	m_checks.replace(index, status);
}
