#include <QFileInfo>
#include "spherefile.h"


SphereFile::SphereFile(QObject *parent) {}

SphereFile::~SphereFile() {}

void SphereFile::createNew() {}

bool SphereFile::open(QString filename) {}

bool SphereFile::save() {}

QString SphereFile::fileName() {
	return this->file->fileName();
}

QString SphereFile::getBaseName() {
	return QFileInfo(this->file->fileName()).baseName();
}
