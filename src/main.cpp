#include <QApplication>
#include <QSettings>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
	Q_INIT_RESOURCE(qsires);
	QApplication a(argc, argv);
	qInfo("Starting QtSphere IDE v%s", VERSION);
	a.setOrganizationName("Spherical");
	a.setOrganizationDomain("spheredev.org");
	a.setApplicationName("QtSphere IDE");
	a.setApplicationVersion(VERSION);
    a.setStyle("fusion");
	MainWindow w;
	w.setWindowTitle(a.applicationDisplayName() + " v" + a.applicationVersion());
	QSettings settings;
	w.setTheme(settings.value("theme").toString());
	if(settings.value("maximized", true).toBool()) {
		w.showMaximized();
	}else {
		QRect geom = settings.value("geometry",QRect(0,50,900,700)).toRect();
		w.setGeometry(geom);
		w.showNormal();
	}
	return a.exec();
}
