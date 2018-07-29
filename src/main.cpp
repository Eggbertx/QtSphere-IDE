#include <QApplication>
#include <QSettings>

#include "util.h"
#include "mainwindow.h"
#include "util.h"

int main(int argc, char *argv[]) {
	Q_INIT_RESOURCE(qsires);
	QApplication a(argc, argv);
	qDebug().nospace() << "Starting QtSphere IDE v" << VERSION;

	a.setOrganizationName("Spherical");
	a.setOrganizationDomain("spheredev.org");
	a.setApplicationName("QtSphere IDE");
	a.setApplicationVersion(VERSION);

	MainWindow w;
	w.setWindowTitle(a.applicationDisplayName() + " v" + a.applicationVersion());
	w.setWindowIcon(QIcon(QPixmap(":/icons/icon.png")));
	QSettings settings;
	w.setTheme(settings.value("theme").toString());
	w.showMaximized();
	return a.exec();
}
