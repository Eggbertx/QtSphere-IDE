#include <QDebug>
#include <QFileInfo>
#include "spherefile.h"


SphereFile::SphereFile(QObject *parent) {}

SphereFile::~SphereFile() {
	//delete this->file;
}

void SphereFile::createNew() {}

bool SphereFile::open(QString filename) {
	(void)filename;
	return true;
}

bool SphereFile::save(QString filename) {
	(void)filename;
	return true;
}

QString SphereFile::fileName() {
	if(this->file) return this->file->fileName();
	return "";
	//return this->file->fileName();
}

QString SphereFile::getBaseName() {
	return QFileInfo(this->file->fileName()).baseName();
}

// read string with no null terminator from file
QString SphereFile::readNextString() {
	uint16_t string_length;
	this->file->read(reinterpret_cast<char*>(&string_length), 2);

	if(string_length == 0) return "";
	QString string_str;
	for(int c = 0; c < string_length; c++) {
		char cc;
		this->file->read(&cc, 1);
		string_str += cc;
	}
	return string_str;
}
