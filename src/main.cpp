#include <QApplication>
#include "util.h"
#include "mainwindow.h"
#include "config.h"
#include "util.h"

int main(int argc, char *argv[]) {
	Q_INIT_RESOURCE(qsires);
	QApplication a(argc, argv);
	qDebug().noquote() << "Starting Qt Sphere IDE v" << VERSION;
	a.setApplicationVersion(VERSION);
	Config* ideConfig = new Config();
	MainWindow w(ideConfig);
	w.setWindowTitle(MAINWINDOW_TITLE);
	ideConfig->loadConfig();
	w.setWindowIcon(QIcon(QPixmap(":/icons/icon.png")));
	w.showMaximized();
	return a.exec();
}
