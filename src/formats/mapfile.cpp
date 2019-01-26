#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRect>
#include <stdio.h>
#include "spherefile.h"
#include "mapfile.h"
#include "formats/tileset.h"
#include "util.h"

MapFile::MapFile(QObject *parent) : SphereFile(parent) {

}

bool MapFile::open(QString filename) {
	this->file = new QFile(filename);

	if(!this->file->open(QIODevice::ReadOnly)) {
		errorBox("ERROR: Could not read file '" + filename + "': " + this->file->errorString());
		return false;
	}

	this->file->read(reinterpret_cast<char*>(&this->header), sizeof(this->header));

	if(memcmp(header.signature, ".rmp", 4) != 0) {
		errorBox("Error: '" + filename + "' is not a valid map file (invalid signature)!");
		this->file->close();
		return false;
	}
	if(header.version != 1) {
		errorBox("Error: '" + filename + "' is not a valid map file (version != 1)!");
		this->file->close();
		return false;
	}
	if(header.num_strings != 3 && header.num_strings != 5 && header.num_strings != 9) {
		errorBox("Error: '" + filename + "' is not a valid map file (invalid strings field)!");
		this->file->close();
		return false;
	}

	this->tilesetFilename = this->readNextString();
	this->musicFilename = this->readNextString();
	this->scriptFilename = this->readNextString();
	if(this->header.num_strings > 3) {
		this->entryScript = this->readNextString();
		this->exitScript = this->readNextString();
	}
	if(this->header.num_strings > 5) {
		this->northScript = this->readNextString();
		this->eastScript = this->readNextString();
		this->southScript = this->readNextString();
		this->westScript = this->readNextString();
	}

	/*qDebug() <<  filename << "data:\n" <<
		"Version:" << header.version << "\n" <<
		"Type:" << header.type << "\n" <<
		"Layers:" << header.num_layers << "\n" <<
		"Entities:" << header.num_entities << "\n" <<
		"Start X:" << header.start_x << "\n" <<
		"Start Y:" << header.start_y << "\n" <<
		"Start layer:" << header.start_layer << "\n" <<
		"Start direction:" << header.start_direction << "\n" <<
		"# strings:" << header.num_strings << "\n" <<
		"# zones:" << header.num_zones << "\n" <<
		"Tileset:" << this->tilesetFilename << "\n" <<
		"Music file:" << this->musicFilename << "\n" <<
		"Script file:" << this->scriptFilename << "\n" <<
		"Entry script:" << this->entryScript << "\n" <<
		"Exit script:" << this->exitScript << "\n" <<
		"North script:" << this->northScript << "\n" <<
		"East script:" << this->eastScript << "\n" <<
		"South script:" << this->southScript << "\n" <<
		"West script:" << this->westScript;*/

	for(int l = 0; l < this->header.num_layers; l++) {
		layer cur_layer;
		cur_layer.index = l;

		this->file->read(reinterpret_cast<char*>(&cur_layer.header), sizeof(cur_layer.header));
		cur_layer.name = this->readNextString();

		/*qDebug().nospace() << "Layer[" << cur_layer.name << "] data:\n" <<
			"Width: " << cur_layer.header.width << "\n" <<
			"Height: " << cur_layer.header.height << "\n" <<
			"Flags: " << cur_layer.header.flags << "\n" <<
			"Parallax X: " << cur_layer.header.parallax_x << "\n" <<
			"Parallax Y: " << cur_layer.header.parallax_y << "\n" <<
			"Scrolling X: " << cur_layer.header.scrolling_x << "\n" <<
			"Scrolling Y: " << cur_layer.header.scrolling_y << "\n" <<
			"Num segments: " << cur_layer.header.num_segments << "\n" <<
			"Reflective: " << cur_layer.header.reflective;*/

		int num_bytes = cur_layer.header.width * cur_layer.header.height;
		for(int b = 0; b < num_bytes; b++) {
			uint16_t tile_index;
			this->file->read(reinterpret_cast<char*>(&tile_index), 2);
			cur_layer.tiles.append(tile_index);
		}
		cur_layer.visible = true;
		this->layers.append(cur_layer);

		for(int s = 0; s < cur_layer.header.num_segments; s++) {
			obs_segment segment;
			segment.layer = l;
			this->file->read(reinterpret_cast<char*>(&segment.x1), sizeof(segment.x1));
			this->file->read(reinterpret_cast<char*>(&segment.x2), sizeof(segment.x2));
			this->file->read(reinterpret_cast<char*>(&segment.y1), sizeof(segment.y1));
			this->file->read(reinterpret_cast<char*>(&segment.y2), sizeof(segment.y2));
			this->segments.append(segment);
		}
	}

	for(int e = 0; e < this->header.num_entities; e++) {
		entity cur_entity;
		this->file->read(reinterpret_cast<char*>(&cur_entity.x), sizeof(cur_entity.x));
		this->file->read(reinterpret_cast<char*>(&cur_entity.y), sizeof(cur_entity.y));
		this->file->read(reinterpret_cast<char*>(&cur_entity.layer), sizeof(cur_entity.layer));
		this->file->read(reinterpret_cast<char*>(&cur_entity.type), sizeof(cur_entity.type));

		skipBytes(this->file, 8);
		if(cur_entity.type > 2) {
			warningBox("Entity #" + QString::number(e+1) + " does not have a valid type, it will be treated like a trigger.");
		}
		switch(cur_entity.type) {
		case 1: {
			cur_entity.name = this->readNextString();
			cur_entity.spriteset = this->readNextString();
			this->file->read(reinterpret_cast<char*>(&cur_entity.num_scripts), sizeof(cur_entity.num_scripts));
			cur_entity.on_create = this->readNextString();
			cur_entity.on_destroy = this->readNextString();
			cur_entity.on_activate_touch = this->readNextString();
			cur_entity.on_activate_talk = this->readNextString();
			cur_entity.generate_commands = this->readNextString();
			skipBytes(this->file, 16);
			/*qDebug() << "Entity name:" << cur_entity.name << "\n" <<
						"Entity spriteset:" << cur_entity.spriteset;*/
		}
		break;
		case 2: {
			cur_entity.trigger_function = this->readNextString();
		}
		break;
		}
		this->entities.append(cur_entity);
	}
	for(int z = 0; z < this->header.num_zones; z++) {
		zone cur_zone;
		this->file->read(reinterpret_cast<char*>(&cur_zone.x1), sizeof(cur_zone.x1));
		this->file->read(reinterpret_cast<char*>(&cur_zone.x2), sizeof(cur_zone.x2));
		this->file->read(reinterpret_cast<char*>(&cur_zone.y1), sizeof(cur_zone.y1));
		this->file->read(reinterpret_cast<char*>(&cur_zone.y2), sizeof(cur_zone.y2));
		this->file->read(reinterpret_cast<char*>(&cur_zone.layer), sizeof(cur_zone.layer));
		this->file->read(reinterpret_cast<char*>(&cur_zone.reactivation_delay), sizeof(cur_zone.reactivation_delay));
		cur_zone.function = this->readNextString();
		this->zones.append(cur_zone);
	}

	this->tileset = new Tileset(this);
	if(this->tilesetFilename == "") {
		this->tileset->readBytes(this->file->readAll());
	} else {
		QFileInfo fi(this->fileName());
		this->tileset->open(fi.dir().absoluteFilePath(this->tilesetFilename));
	}
	this->file->close();
	return true;
}

bool MapFile::save(QString filename) {
	bool success = false;

	return success;
}

QRect* MapFile::largestLayerRect() {
	int largestWidth = 0;
	int largestHeight = 0;
	for(int i = 0; i < this->layers.length(); i++) {
		layer_header cur_header = this->layers.at(i).header;
		if(cur_header.width > largestWidth && cur_header.height > largestHeight) {
			largestWidth = cur_header.width;
			largestHeight = cur_header.height;
		}
	}
	return new QRect(0, 0, largestWidth, largestHeight);
}

QSize MapFile::tileSize() {
	return QSize(this->tileset->header.tile_width, this->tileset->header.tile_height);
}

QSize MapFile::mapSize() {
	QSize tSize = this->tileSize();

	return tSize;
}

QList<MapFile::layer> MapFile::getLayers() {
	return this->layers;
}

MapFile::layer* MapFile::getLayer(int index) {
	if(index > 0 && index < this->layers.length()) return &this->layers[index];
	return nullptr;
}

int MapFile::removeLayer(int index) {
	this->layers.removeAt(index);
	return this->layers.length();
}

int MapFile::numLayers() {
	return this->layers.length();
}

int MapFile::getTileIndex(int l, int x, int y) {
	if(l >= this->layers.length()) return -1;
	layer cur_layer = this->layers.at(l);
	if(x >= cur_layer.header.width || y >= cur_layer.header.height) return -1;

	int arr_index =  x + cur_layer.header.width * y;
	if(arr_index >= cur_layer.tiles.length()) return -1;

	return cur_layer.tiles.at(arr_index);
}

QList<MapFile::entity> MapFile::getEntities(int layer) {
	QList<MapFile::entity> layerEntities;
	foreach(entity e, this->entities) {
		if(e.layer == layer || layer == -1) layerEntities.append(e);
	}
	return this->entities;
}

MapFile::entity* MapFile::getEntity(int index) {
	if(index > this->entities.length()) return nullptr;
	return &this->entities[index];
}
