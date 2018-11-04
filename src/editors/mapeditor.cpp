#include <QDir>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QLabel>

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
	ui->layersTable->clear();
	ui->layersTable->setRowCount(numLayers);
	for(int l = numLayers; l > 0; l--) {
		int i = numLayers - l;
		MapFile::layer cur_layer = layers.at(i);

		QLabel* eyeLabel = new QLabel();
		//eyeLabel->setScaledContents(true);
		eyeLabel->setPixmap(QPixmap(":/icons/eye.png"));
		eyeLabel->setAlignment(Qt::AlignCenter);
		ui->layersTable->setCellWidget(l-1,0,eyeLabel);

		ui->layersTable->setItem(l-1,1, new QTableWidgetItem(cur_layer.name));

		QLabel* deleteLabel = new QLabel("X");
		deleteLabel->setStyleSheet("color:red;");
		deleteLabel->setAlignment(Qt::AlignCenter);
		ui->layersTable->setCellWidget(l-1,2,deleteLabel);

		for(int t = 0; t < cur_layer.tiles.length(); t++) {
			uint16_t cur_tile = cur_layer.tiles.at(t);
			QGraphicsPixmapItem* tilePixmap = this->mapScene->addPixmap(QPixmap::fromImage(attachedTileset.getImage(cur_tile)));

			int x = t % cur_layer.header.width;
			int y = (t - x) / cur_layer.header.width;
			tilePixmap->setOffset(x * attachedTileset.header.tile_width,y * attachedTileset.header.tile_height);
		}
	}
	ui->mapView->setScene(this->mapScene);
	QList<QImage> images = attachedTileset.getTileImages();

	for(int i = 0; i < images.length(); i++) {
		QGraphicsPixmapItem* tilePixmap = this->tilesScene->addPixmap(QPixmap::fromImage(images.at(i)));
		tilePixmap->setOffset(i*attachedTileset.header.tile_width+16,10)		;
	}
	ui->tilesetView->setScene(this->tilesScene);
	ui->tilesetBox->setTitle("Tiles (" + QString::number(attachedTileset.header.num_tiles) + ")");
	return true;
}

void MapEditor::on_layersTable_cellClicked(int row, int column) {
	switch(column) {
		case 0: {
			QWidget* item = ui->layersTable->cellWidget(row,column);
			QLabel* eyeLabel = dynamic_cast<QLabel*>(item);
			MapFile::layer* toggled_layer = this->mapFile->getLayer(row);
			toggled_layer->visible = !toggled_layer->visible;

			if(!toggled_layer->visible)
				eyeLabel->setPixmap(QPixmap(":/icons/eye-closed.png"));
			else
				eyeLabel->setPixmap(QPixmap(":/icons/eye.png"));
		}
		break;
		case 2: {
			if(ui->layersTable->rowCount() > 1)
				ui->layersTable->removeRow(row);
		}
		break;
		default:
		break;
	}
}
