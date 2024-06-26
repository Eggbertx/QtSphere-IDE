#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QRect>
#include <QXmlStreamReader>
#include <stdio.h>

#include "formats/spherefile.h"
#include "formats/mapfile.h"
#include "formats/tileset.h"
#include "util.h"

MapFile::MapFile(QObject *parent) : SphereFile(parent) {
	m_tileset = nullptr;
}

MapFile::~MapFile() {
	if(m_tileset != nullptr) {
		if(m_tileset->isOpen()) m_tileset->close();
		delete m_tileset;
	}
	close();
}

bool MapFile::open(QString filename, QIODevice::OpenMode flags) {
	reset();
	if(!SphereFile::open(filename, flags)) {
		errorBox("ERROR: Could not read file '" + filename + "': " + m_file->errorString());
		close();
		return false;
	}

	readFile(m_file, &m_header, sizeof(m_header));
	if(memcmp(m_header.signature, ".rmp", 4) != 0) {
		errorBox("Error: '" + filename + "' is not a valid map file (invalid signature)!");
		close();
		return false;
	}
	if(m_header.version != 1) {
		errorBox("Error: '" + filename + "' is not a valid map file (version != 1)!");
		close();
		return false;
	}
	if(m_header.num_strings != 3 && m_header.num_strings != 5 && m_header.num_strings != 9) {
		errorBox("Error: '" + filename + "' is not a valid map file (invalid strings field)!");
		close();
		return false;
	}
	m_tilesetFilename = readNextString();
	m_musicFilename = readNextString();
	m_scriptFilename = readNextString();
	if(m_header.num_strings > 3) {
		m_entryScript = readNextString();
		m_exitScript = readNextString();
	}
	if(m_header.num_strings > 5) {
		m_northScript = readNextString();
		m_eastScript = readNextString();
		m_southScript = readNextString();
		m_westScript = readNextString();
	}

	for(int l = 0; l < m_header.num_layers; l++) {
		layer cur_layer;
		cur_layer.index = l;
		readFile(m_file, &cur_layer.header, sizeof(cur_layer.header));
		cur_layer.name = readNextString();

		int num_bytes = cur_layer.header.width * cur_layer.header.height;
		for(int b = 0; b < num_bytes; b++) {
			uint16_t tile_index;
			readFile(m_file, &tile_index, 2);
			cur_layer.tiles.append(tile_index);
		}
		m_layers.append(cur_layer);

		for(uint32_t s = 0; s < cur_layer.header.num_segments; s++) {
			obs_segment segment;
			segment.layer = l;
			readFile(m_file, &segment.x1, sizeof(segment.x1));
			readFile(m_file, &segment.x2, sizeof(segment.x2));
			readFile(m_file, &segment.y1, sizeof(segment.y1));
			readFile(m_file, &segment.y2, sizeof(segment.y2));
			m_segments.append(segment);
		}
	}

	for(int e = 0; e < m_header.num_entities; e++) {
		entity cur_entity;
		readFile(m_file, &cur_entity.x, sizeof(cur_entity.x));
		readFile(m_file, &cur_entity.y, sizeof(cur_entity.y));
		readFile(m_file, &cur_entity.layer, sizeof(cur_entity.layer));
		readFile(m_file, &cur_entity.type, sizeof(cur_entity.type));

		skipBytes(m_file, 8);
		if(cur_entity.type > 2) {
			warningBox("Entity #" + QString::number(e+1) + " does not have a valid type, it will be treated like a trigger.");
		}
		switch(cur_entity.type) {
		case 1: {
			cur_entity.name = readNextString();
			cur_entity.spriteset = readNextString();
			readFile(m_file, &cur_entity.num_scripts, sizeof (cur_entity.num_scripts));
			cur_entity.on_create = readNextString();
			cur_entity.on_destroy = readNextString();
			cur_entity.on_activate_touch = readNextString();
			cur_entity.on_activate_talk = readNextString();
			cur_entity.generate_commands = readNextString();
			skipBytes(m_file, 16);
			/*qDebug() << "Entity name:" << cur_entity.name << "\n" <<
						"Entity spriteset:" << cur_entity.spriteset;*/
		}
		break;
		case 2: {
			cur_entity.trigger_function = readNextString();
		}
		break;
		}
		m_entities.append(cur_entity);
	}
	for(int z = 0; z < m_header.num_zones; z++) {
		zone cur_zone;
		readFile(m_file, &cur_zone.x1, sizeof(cur_zone.x1));
		readFile(m_file, &cur_zone.x2, sizeof(cur_zone.x2));
		readFile(m_file, &cur_zone.y1, sizeof(cur_zone.y1));
		readFile(m_file, &cur_zone.y2, sizeof(cur_zone.y2));
		readFile(m_file, &cur_zone.layer, sizeof(cur_zone.layer));
		readFile(m_file, &cur_zone.reactivation_delay, sizeof(cur_zone.reactivation_delay));
		cur_zone.function = readNextString();
		//qDebug("cur_zone.function: %s\n// end", cur_zone.function.toStdString().c_str());
		m_zones.append(cur_zone);
	}

	m_tileset = new Tileset(this);
	if(m_tilesetFilename == "") {
		// qDebug("Position: %d\n", m_file->pos());
		if(!m_tileset->readBytes(m_file->readAll())) return false;
	} else {
		QFileInfo fi(fileName());
		m_tileset->open(fi.dir().absoluteFilePath(m_tilesetFilename));
	}
	return true;
}

bool MapFile::save(QString filename) {
	bool success = false;

	return success;
}

bool MapFile::openTiledMap(QString filename) {
	bool success = false;
	QXmlStreamReader* xmlReader = nullptr;
	QFile* mapFile = nullptr;
	if(filename == "") {
		filename = QFileDialog::getOpenFileName(nullptr, "Open file", "",
			"Tiled XML map files (*.tmx *.xml *.json);;"
			"All files (*.*)"
		);
		if(filename == "") return success;
	}

	int mapType = MapFile::TiledUnknownFormat;
	if(filename.endsWith("tmx", Qt::CaseInsensitive) || filename.endsWith("xml", Qt::CaseInsensitive)) {
		mapType = MapFile::TiledTMX;
	} else if(filename.endsWith("json", Qt::CaseInsensitive)) {
		mapType = MapFile::TiledJSON;
	} else {
		errorBox("This doesn't appear to be a Tiled map (must have a .tmx, .xml, or .json extension)");
		return false;
	}
	mapFile = new QFile(filename);
	if(!mapFile->open(QFile::ReadOnly)) {
		errorBox("Failed to open Tiled map (" + filename + "):" + mapFile->errorString());
		goto cleanup;
	}

	switch(mapType) {
	case MapFile::TiledTMX:
		xmlReader = new QXmlStreamReader(mapFile->readAll());
		if(!xmlReader->readNextStartElement() || xmlReader->name().toString() != "map") {
			errorBox("Failed read valid XML from map file.");
			goto cleanup;
		}
		QXmlStreamAttributes attributes = xmlReader->attributes();
		bool ok;
		int width = attributes.value("width").toInt(&ok);
		if(!ok) width = -1;
		int height = attributes.value("height").toInt(&ok);
		if(!ok) height = -1;

		if(width < 0 || height < 0) {
			errorBox("Error getting Tiled map size");
			goto cleanup;
		}
		resizeAllLayers(width, height);

		foreach(QXmlStreamAttribute attribute, attributes) {

            QStringView name = attribute.name();
            QStringView value = attribute.value();
			qDebug() << name << ":" << value;
		}
	}
cleanup:
	if(mapFile != nullptr && mapFile->isOpen()) {
		mapFile->close();
		delete mapFile;
	}
	if(xmlReader != nullptr)
		delete xmlReader;
	return success;
}

QRect* MapFile::largestLayerRect() {
	int largestWidth = 0;
	int largestHeight = 0;
	for(int i = 0; i < m_layers.length(); i++) {
		layer_header cur_header = m_layers.at(i).header;
		if(cur_header.width > largestWidth && cur_header.height > largestHeight) {
			largestWidth = cur_header.width;
			largestHeight = cur_header.height;
		}
	}
	return new QRect(0, 0, largestWidth, largestHeight);
}

QSize MapFile::getTileSize() {
	return m_tileset->getTileSize();
}

Tileset* MapFile::getTileset() {
	return m_tileset;
}

QString MapFile::getScript(MapFile::ScriptType type) {
	switch(type) {
		case MapFile::Entry:
			return m_entryScript;
		case MapFile::Exit:
			return m_exitScript;
		case MapFile::LeaveNorth:
			return m_northScript;
		case MapFile::LeaveEast:
			return m_eastScript;
		case MapFile::LeaveSouth:
			return m_southScript;
		case MapFile::LeaveWest:
			return m_westScript;
	}
	return "";
}

void MapFile::setScript(MapFile::ScriptType type, QString text) {
	switch(type) {
		case MapFile::Entry:
			m_entryScript = text;
			break;
		case MapFile::Exit:
			m_exitScript = text;
			break;
		case MapFile::LeaveNorth:
			m_northScript = text;
			break;
		case MapFile::LeaveEast:
			m_eastScript = text;
			break;
		case MapFile::LeaveSouth:
			m_southScript = text;
			break;
		case MapFile::LeaveWest:
			m_westScript = text;
			break;
	}
}

QList<MapFile::layer> MapFile::getLayers() {
	return m_layers;
}

MapFile::layer* MapFile::getLayer(int index) {
	if(index > -1 && index < m_layers.length()) return &m_layers[index];
	return nullptr;
}

int MapFile::removeLayer(int index) {
	m_layers.removeAt(index);
	return m_layers.length();
}

void MapFile::resizeAllLayers(int width, int height) {
	m_header.num_layers = numLayers();
	foreach (layer curLayer, m_layers) {
		if(width > 0) curLayer.header.width = width;
		if(width > 0) curLayer.header.height = height;
	}
}

void MapFile::resizeAllLayers(QSize size) {
	resizeAllLayers(size.width(), size.height());
}

int MapFile::numLayers() {
	return m_layers.length();
}

bool MapFile::isLayerVisible(int layer) {
	if(m_header.num_layers <= layer) return false;
	return (m_layers[layer].header.flags & 1) == 0;
}

void MapFile::setLayerVisible(int layer, bool visible) {
	if(m_header.num_layers <= layer) return;
	if(isLayerVisible(layer) != visible) {
		m_layers[layer].header.flags ^= 1;
	}
}

int MapFile::getTileIndex(int l, int x, int y) {
	if(l >= m_layers.length()) return -1;
	layer cur_layer = m_layers.at(l);
	if(x >= cur_layer.header.width || y >= cur_layer.header.height) return -1;

	int arr_index =  x + cur_layer.header.width * y;
	if(arr_index >= cur_layer.tiles.length()) return -1;

	return cur_layer.tiles.at(arr_index);
}

QList<MapFile::entity> MapFile::getEntities(int layer) {
	QList<MapFile::entity> layerEntities;
	foreach(entity e, m_entities) {
		if(e.layer == layer || layer == -1) layerEntities.append(e);
	}
	return m_entities;
}

MapFile::entity* MapFile::getEntity(int index) {
	if(index > m_entities.length()) return nullptr;
	return &m_entities[index];
}

void MapFile::reset() {
	m_header.signature[0] = '.';
	m_header.signature[1] = 'r';
	m_header.signature[2] = 'm';
	m_header.signature[3] = 'p';
	m_header.version = 1;
	m_header.num_layers = 0;
	m_header.num_entities = 0;
	m_header.start_x = 0;
	m_header.start_y = 0;
	m_header.start_layer = 0;
	m_header.start_direction = MapFile::South;
	m_header.num_strings = 0;
	m_header.num_zones = 0;
	m_musicFilename = "";
	m_tilesetFilename = "";
	if(m_tileset != nullptr) {
		m_tileset->close();
		delete m_tileset;
	}
	m_scriptFilename = "";
	m_entryScript = "";
	m_exitScript = "";
	m_northScript = "";
	m_eastScript = "";
	m_southScript = "";
	m_westScript = "";
	m_layers.clear();
	m_segments.clear();
	m_entities.clear();
	m_zones.clear();
}
