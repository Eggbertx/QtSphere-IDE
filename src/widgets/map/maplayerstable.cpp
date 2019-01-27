#include <QMenu>
#include "widgets/map/maplayerstable.h"

MapLayersTable::MapLayersTable(QWidget* parent): QTableWidget(parent) {
	this->layerMenu = new QMenu(this);
	this->layerMenu->addAction("Insert layer");
	this->layerMenu->addAction("Delete layer");
	this->layerMenu->addAction("Duplicate layer");
	this->layerMenu->addSeparator();
	this->layerMenu->addAction("Toggle lock layer");
	this->layerMenu->addAction("Properties...");

}

void MapLayersTable::attachMap(MapFile* map) {
	this->map = map;
}



void MapLayersTable::mousePressEvent(QMouseEvent *event) {

}
