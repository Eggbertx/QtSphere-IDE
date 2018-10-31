#include <QDebug>
#include "mapeditor.h"
#include "ui_mapeditor.h"
#include "sphereeditor.h"
#include "formats/mapfile.h"
#include "util.h"

MapEditor::MapEditor(QWidget *parent) : SphereEditor(parent), ui(new Ui::MapEditor) {
	ui->setupUi(this);
	this->mapFile = new MapFile(this);
}

MapEditor::~MapEditor() {
	delete ui;
	delete this->mapFile;
}

bool MapEditor::openFile(QString filename) {
	if(!this->mapFile->open(filename)) return false;
	return this->attach(this->mapFile);
}

bool MapEditor::attach(MapFile* attachedMap) {
	infoBox("Starting map");
	QList<MapFile::layer> layers = attachedMap->getLayers();

	return true;
}
