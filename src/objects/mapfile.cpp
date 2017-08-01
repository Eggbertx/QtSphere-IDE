#include <QFile>
#include <QByteArray>
#include <QDebug>

#include "mapfile.h"


MapFile::MapFile(QWidget *parent) : QGraphicsView(parent) {

}

bool MapFile::loadMap(QString filename) {
	QFile rmpFile(filename);
	if(!rmpFile.open(QIODevice::ReadOnly)) throw "ERROR: Could not read file " + filename;
	rmpFile.open(QIODevice::Text);
	fileHeader header_struct;
	rmpFile.readLine(header_struct.signature, 1);
	qDebug() << header_struct.signature;
}
