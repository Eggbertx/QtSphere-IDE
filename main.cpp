#include <QApplication>

#include "util.h"
#include "mainwindow.h"
#include "config.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	Config ideConfig;
	ideConfig.loadConfig();

	MainWindow w;

	w.showMaximized();
	return a.exec();
}
