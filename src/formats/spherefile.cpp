#include <QFileInfo>
#include "formats/spherefile.h"
#include "util.h"

SphereFile::SphereFile(QObject* parent): QObject(parent) {
	m_file = nullptr;
}

SphereFile::~SphereFile() {
	if(isOpen())
		close();
}

void SphereFile::createNew() {}

bool SphereFile::open(QString filename, QFile::OpenMode flags) {
	if(m_file == nullptr) m_file = new QFile(filename);
	else m_file->setFileName(filename);
	m_filename = filename;
	return m_file->open(flags);
}

bool SphereFile::isOpen() {
	return (m_file != nullptr && m_file->isOpen());
}

void SphereFile::close() {
	if(m_file != nullptr) {
		if(m_file->isOpen()) m_file->close();
		delete m_file;
	}
	m_filename = "";
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
	if(!isOpen()) return "";
	uint16_t string_length;
	m_file->read(reinterpret_cast<char*>(&string_length), 2);
	if(string_length == 0) return "";

	char* str = reinterpret_cast<char*>(malloc(string_length));
	m_file->read(str, string_length);
	QString string_str = QString(str);
	string_str.truncate(string_length); // There's probably a better/safer way, but this works (for now)
	return string_str;
}

void SphereFile::addString(QString str) {
	if(!isOpen()) return;
	uint16_t string_length = str.length();
	m_file->write(reinterpret_cast<char*>(string_length), 2);
	m_file->write(str.toStdString().c_str(), string_length);
}
