#include <QByteArray>
#include <QDataStream>
#include "formats/spherefile.h"
#include "formats/tileset.h"
#include "util.h"

Tileset::Tileset(QObject *parent) : SphereFile (parent) {

}

bool Tileset::readBytes(QByteArray arr) {
	QDataStream stream(&arr, QIODevice::ReadOnly);
	stream.readRawData(reinterpret_cast<char*>(&m_header), sizeof (m_header));
	if(memcmp(m_header.signature, ".rts", 4) != 0) {
		errorBox("Error: '" + fileName() + "' is not a valid tileset file (invalid signature)!");
		return false;
	}
	if(m_header.version != 1) {
		errorBox("Error: Invalid tileset version (got " + QString::number(m_header.version) + ",  must be 1)!");
		return false;
	}
	if(m_header.tile_width < 1 || m_header.tile_height < 1) {
		errorBox("Error: Invalid tile resolution (Must be > 0)!");
		return false;
	}

	for(int t = 0; t < m_header.num_tiles; t++) {
		tile cur_tile;
		cur_tile.index = t;
		int num_img_bytes = m_header.tile_width * m_header.tile_height * 4;
		unsigned char* image_bytes = new unsigned char[num_img_bytes];

		stream.readRawData(reinterpret_cast<char*>(image_bytes), num_img_bytes);

		cur_tile.image = QImage(
			image_bytes,
			m_header.tile_width, m_header.tile_height,
			QImage::Format_RGBA8888
		).copy(0,0,m_header.tile_width,m_header.tile_height);
		m_tiles.append(cur_tile);
	}
	for(int t = 0; t < m_header.num_tiles; t++) {
		stream.readRawData(reinterpret_cast<char*>(&m_tiles[t].info), sizeof(m_tiles[t].info));
	}
	return true;
}

bool Tileset::open(QString filename) {
	SphereFile::open(filename);
	QByteArray arr;

	m_file = new QFile(filename);
	if(!m_file->open(QIODevice::ReadOnly)) {
		errorBox("ERROR: Could not open file " + filename + ": " + m_file->errorString());
		return false;
	}
	return readBytes(m_file->readAll());
}

bool Tileset::save(QString filename) {
	SphereFile::save(filename);
	return true;
}

int Tileset::numTiles() {
	return m_header.num_tiles;
}

QSize Tileset::getTileSize() {
	return QSize(m_header.tile_width, m_header.tile_height);
}

QImage Tileset::getImage(int index) {
	return m_tiles.at((uint8_t)index).image;
}

QList<QImage> Tileset::getTileImages() {
	QList<QImage> images = QList<QImage>();
	for(int i = 0; i < m_tiles.length(); i++)
		images.append(m_tiles.at(i).image);
	return images;
}
