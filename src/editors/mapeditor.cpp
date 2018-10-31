#include <QDebug>
#include <QGraphicsScene>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QWidget>
#include "mapeditor.h"
#include "ui_mapeditor.h"
#include "sphereeditor.h"
#include "formats/mapfile.h"
#include "util.h"

MapEditor::MapEditor(QWidget *parent) : SphereEditor(parent), ui(new Ui::MapEditor) {
	ui->setupUi(this);
	this->mapFile = new MapFile(this);
	this->mapScene = new QGraphicsScene(ui->mapView);
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

		int num_tiles = cur_layer.tiles.length();
		for(int t = 0; t < num_tiles; t++) {
			uint8_t cur_tile = cur_layer.tiles.at(t);

		}

		ui->mapView->setScene(this->mapScene);
	}
	return true;
}
