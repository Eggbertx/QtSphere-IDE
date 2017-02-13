#include <QApplication>

#include "util.h"
#include "mainwindow.h"
#include "config.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	Config ideConfig;
    MainWindow w;

    ideConfig.loadConfig();
    w.showMaximized();
	return a.exec();
}
