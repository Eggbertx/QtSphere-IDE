#include "qsiproject.h"

QSIProject::QSIProject(QString path, QObject *parent) : QObject(parent) {
	this->path = path;
}

QString QSIProject::getPath() {
	return this->path;
}

QString QSIProject::getCompiler() {
	return this->compler;
}

void QSIProject::setCompiler(QString set) {
	this->compler = set;
}
