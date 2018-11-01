#include "spherefile.h"
#include "tileset.h"
#include "util.h"

Tileset::Tileset(QObject *parent) : SphereFile (parent) {

}

bool Tileset::open(QString filename) {
	this->file = new QFile(filename);
	if(!this->file->open(QIODevice::ReadOnly)) {
		errorBox("ERROR: Could not read file " + filename + ": " + this->file->errorString());
		return false;
	}

	this->file->read(reinterpret_cast<char*>(&this->header), sizeof(this->header));
	if(memcmp(this->header.signature, ".rts", 4) != 0) {
		errorBox("Error: " + QString(filename) + " is not a valid tileset file (invalid signature)!");
		this->file->close();
		return false;
	}
	if(this->header.version != 1) {
		errorBox("Error: Invalid tileset version (must be 1)!");
		this->file->close();
		return false;
	}
	if(this->header.tile_width < 1 || this->header.tile_height < 1) {
		errorBox("Error: Invalid tile resolution (Must be > 0)!");
		this->file->close();
		return false;
	}

	for(int t = 0; t < this->header.num_tiles; t++) {
		tile cur_tile;
		cur_tile.index = t;
		int num_img_bytes = this->header.tile_width * this->header.tile_height * 4;
		unsigned char* image_bytes = new unsigned char[num_img_bytes];

		this->file->read(reinterpret_cast<char*>(image_bytes), num_img_bytes);

		cur_tile.image = QImage(
			image_bytes,
			this->header.tile_width, this->header.tile_height,
			QImage::Format_RGBA8888
		).copy(0,0,this->header.tile_width,this->header.tile_height);
		this->tiles.append(cur_tile);
	}
	for(int t = 0; t < this->header.num_tiles; t++) {
		this->file->read(reinterpret_cast<char*>(&this->tiles[t].info), sizeof(this->tiles[t].info));
	}

	return true;
}

bool Tileset::save(QString filename) {

}

QImage Tileset::getImage(int index) {
	return this->tiles.at((uint8_t)index).image;
}

QList<QImage> Tileset::getTileImages() {
	QList<QImage> images = QList<QImage>();
	for(int i = 0; i < this->tiles.length(); i++)
		images.append(this->tiles.at(i).image);
	return images;
}
