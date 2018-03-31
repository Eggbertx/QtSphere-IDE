#include "spherefile.h"

SphereFile::SphereFile(QWidget *parent) : QWidget(parent) {
	QString fileExtension;
}

QString SphereFile::fileName() {
	return this->file->fileName();
}

void SphereFile::readFile(QString fn) {
	QString fileExtension;
	this->file = new QFile(fn);
	if (!this->file->open(QIODevice::ReadWrite | QIODevice::Text)) return;
	QByteArray bytes = this->file->readAll();
}

void SphereFile::readFile(QFile *f) {
	this->file = f;
	if (!this->file->open(QIODevice::ReadWrite | QIODevice::Text)) return;

}
