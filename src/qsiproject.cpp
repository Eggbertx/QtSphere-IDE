#include "qsiproject.h"

QSIProject::QSIProject(QString path, QObject *parent) : QObject(parent) {
	this->path = path;
}
