#include <QFileInfo>
#include "formats/spherefile.h"


SphereFile::SphereFile(QObject *parent) {}

SphereFile::~SphereFile() {
	//delete m_file;
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
    if(m_file != nullptr) return m_file->fileName();
	return "";
}

QString SphereFile::getBaseName() {
	return QFileInfo(m_file->fileName()).baseName();
}

// read string with no null terminator from file
QString SphereFile::readNextString() {
	uint16_t string_length;
	m_file->read(reinterpret_cast<char*>(&string_length), 2);

	if(string_length == 0) return "";
	QString string_str;
	for(int c = 0; c < string_length; c++) {
		char cc;
		m_file->read(&cc, 1);
		string_str += cc;
	}
	return string_str;
}
