#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QHBoxLayout>
#include <QIcon>
#include <QPixmap>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QWidget>
#include "mapeditor.h"
#include "ui_mapeditor.h"
#include "sphereeditor.h"
#include "formats/mapfile.h"
#include "formats/tileset.h"
#include "util.h"

MapEditor::MapEditor(QWidget *parent) : SphereEditor(parent), ui(new Ui::MapEditor) {
	ui->setupUi(this);
	this->mapFile = new MapFile(this);
	ui->tilesetView->setAlignment(Qt::AlignLeft|Qt::AlignTop);
	ui->tilesetView->setBackgroundBrush(QBrush(Qt::darkGray, Qt::SolidPattern));
	ui->mapView->setAlignment(Qt::AlignLeft|Qt::AlignTop);
	ui->mapView->setBackgroundBrush(QBrush(Qt::darkGray, Qt::SolidPattern));
	this->mapScene = new QGraphicsScene(ui->mapView);
	this->tilesScene = new QGraphicsScene(ui->tilesetView);
}

MapEditor::~MapEditor() {
	delete ui;
	delete this->mapFile;
	delete this->mapScene;
}

bool MapEditor::openFile(QString filename) {
	if(!this->mapFile->open(filename)) return false;
	return this->attach(this->mapFile);
}

bool MapEditor::attach(MapFile* attachedMap) {
	Tileset attachedTileset;
	QFileInfo mapFi(attachedMap->fileName());
	attachedTileset.open(mapFi.dir().absoluteFilePath(attachedMap->tilesetFilename));

	QList<MapFile::layer> layers = attachedMap->getLayers();
	int numLayers = layers.length();
	ui->layersTable->setRowCount(0);
	for(int l = 0; l < numLayers; l++) {
		MapFile::layer cur_layer = layers.at(numLayers-l-1);

		ui->layersTable->insertRow(l);
		QWidget* eyeWidget = new QWidget();
		QHBoxLayout* eyeLayout = new QHBoxLayout(eyeWidget);
		QPushButton* eyeButton = new QPushButton(QIcon(":/icons/eye.png"),"");
		eyeButton->setStyleSheet("background-color:#00FFFFFF;");
		eyeLayout->addWidget(eyeButton);
		eyeLayout->setAlignment(Qt::AlignCenter);
		eyeLayout->setContentsMargins(0,0,0,0);
		eyeWidget->setLayout(eyeLayout);
		ui->layersTable->setCellWidget(l,0,eyeWidget);

		ui->layersTable->setItem(l,1, new QTableWidgetItem(cur_layer.name));

		QWidget* deleteLayerWidget = new QWidget();
		QHBoxLayout* deleteLayout = new QHBoxLayout(deleteLayerWidget);
		QPushButton* deleteLayerBtn = new QPushButton("X");
		deleteLayerBtn->setStyleSheet("color:red; background-color:#00FFFFFF;");
		deleteLayout->addWidget(deleteLayerBtn);
		deleteLayout->setAlignment(Qt::AlignCenter);
		deleteLayout->setContentsMargins(0, 0, 0, 0);
		deleteLayerWidget->setLayout(deleteLayout);
		ui->layersTable->setCellWidget(l,2,deleteLayerWidget);

		for(int t = 0; t < cur_layer.tiles.length(); t++) {
			uint16_t cur_tile = cur_layer.tiles.at(t);
			QGraphicsPixmapItem* tilePixmap = this->mapScene->addPixmap(QPixmap::fromImage(attachedTileset.getImage(cur_tile)));
			tilePixmap->setZValue(l);

			int x = t % cur_layer.header.width;
			int y = (t - x) / cur_layer.header.width;
			tilePixmap->setOffset(x * attachedTileset.header.tile_width,y * attachedTileset.header.tile_height);
		}
		ui->tilesetBox->setTitle("Tiles (" + QString::number(attachedTileset.header.num_tiles) + ")");
	}
	ui->mapView->setScene(this->mapScene);
	QList<QImage> images = attachedTileset.getTileImages();

	for(int i = 0; i < images.length(); i++) {
		QGraphicsPixmapItem* tilePixmap = this->tilesScene->addPixmap(QPixmap::fromImage(images.at(i)));
		tilePixmap->setOffset(i*attachedTileset.header.tile_width+16,10)		;
	}
	ui->tilesetView->setScene(this->tilesScene);
	return true;
}
