#include "spherefile.h"

SphereFile::SphereFile(QWidget *parent) : QWidget(parent) {
    QString fileExtension;

}



void SphereFile::readFile(QString fn) {
    QString fileExtension;
	this->file = new QFile(fn);
	if (!this->file->open(QIODevice::ReadWrite | QIODevice::Text)) return;
	QByteArray bytes = this->file->readAll();
}
