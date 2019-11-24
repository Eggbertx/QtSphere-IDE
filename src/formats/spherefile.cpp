#include <QFileInfo>
#include "formats/spherefile.h"


SphereFile::SphereFile(QObject *parent) {}

SphereFile::~SphereFile() {
	//delete m_file;
}

void SphereFile::createNew() {}

bool SphereFile::open(QString filename) {
	m_filename = filename;
	return true;
}

bool SphereFile::save(QString filename) {
	m_filename = filename;
	return true;
}

QString SphereFile::fileName() {
	return m_filename;
}

QString SphereFile::getBaseName() {
	return QFileInfo(m_filename).baseName();
}

// read string with no null terminator from file
QString SphereFile::readNextString() {
	uint16_t string_length;
	m_file->read(reinterpret_cast<char*>(&string_length), 2);

	if(string_length == 0) return "honk";
	char* str = (char*)malloc(string_length);
	m_file->read(str, string_length);
	QString string_str = QString(str);
	return string_str;
}
