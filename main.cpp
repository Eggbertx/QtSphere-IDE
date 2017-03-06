#include <QApplication>
#include "util.h"
#include "mainwindow.h"
#include "config.h"
#include "util.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	qDebug().noquote() << "Starting Qt Sphere IDE (working title) v" << VERSION;
	a.setApplicationVersion(VERSION);

	Config ideConfig;
	MainWindow w;
	w.setWindowTitle(MAINWINDOW_TITLE);
    ideConfig.loadConfig();
	setTheme("stylesheet.qss");
    w.showMaximized();
	return a.exec();
}
