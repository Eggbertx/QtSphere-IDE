#include <QFileInfo>
#include "spherefile.h"


SphereFile::SphereFile(QObject *parent) {}

SphereFile::~SphereFile() {
	if(this->file != nullptr) {
		if(this->file->isOpen())
			this->file->close();
		delete this->file;
	}
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
	return this->file->fileName();
}

QString SphereFile::getBaseName() {
	return QFileInfo(this->file->fileName()).baseName();
}
