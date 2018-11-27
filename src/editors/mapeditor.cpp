#include <QDir>
#include <QFileInfo>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>

#include "mapeditor.h"
#include "ui_mapeditor.h"
#include "sphereeditor.h"
#include "formats/mapfile.h"
#include "formats/tileset.h"
#include "util.h"

MapEditor::MapEditor(QWidget *parent) : SphereEditor(parent), ui(new Ui::MapEditor) {
	ui->setupUi(this);
	this->menuBar = new QToolBar();
	this->menuBar->setFixedHeight(32);

	this->pencilMenu = new QMenu(this);

	QList<QAction*> pencilActions({
		new QAction(QIcon(":/icons/1x1grid.png"), "1x1"),
		new QAction(QIcon(":/icons/3x3grid.png"), "3x3"),
		new QAction(QIcon(":/icons/5x5grid.png"), "5x5")
	});

	this->pencilSize = 1;
	this->pencilMenu->addActions(pencilActions);
	this->pencilMenu->setDefaultAction(pencilActions.at(0));

	QToolButton* pencilMenuButton = new QToolButton();
	pencilMenuButton->setText("Pencil");
	pencilMenuButton->setIcon(QIcon(":/icons/pencil.png"));
	pencilMenuButton->setMenu(this->pencilMenu);
	pencilMenuButton->setPopupMode(QToolButton::MenuButtonPopup);
	this->menuBar->addWidget(pencilMenuButton);
	connect(this->pencilMenu, SIGNAL(triggered(QAction*)), this, SLOT(setPencilSize(QAction*)));

	QAction* lineToolButton = new QAction("Line");
	lineToolButton->setIcon(QIcon(":/icons/linetool.png"));
	this->menuBar->addAction(lineToolButton);

	QAction* rectToolButton = new QAction("Rectangle");
	rectToolButton ->setIcon(QIcon(":/icons/rectangletool.png"));
	this->menuBar->addAction(rectToolButton );

	QAction* fillToolButton = new QAction("Fill layer");
	fillToolButton->setIcon(QIcon(":/icons/paintbucket.png"));
	this->menuBar->addAction(fillToolButton);

	QAction* tilePickerButton = new QAction("Select tile");
	tilePickerButton ->setIcon(QIcon(":/icons/dropper.png"));
	this->menuBar->addAction(tilePickerButton );

	ui->mapViewLayout->setMenuBar(this->menuBar);
	this->mapFile = new MapFile(this);
	ui->tilesetView->setAlignment(Qt::AlignLeft|Qt::AlignTop);
	ui->tilesetView->setBackgroundBrush(QBrush(Qt::darkGray, Qt::SolidPattern));
	ui->mapView->setAlignment(Qt::AlignLeft|Qt::AlignTop);
	ui->mapView->setBackgroundBrush(QBrush(Qt::darkGray, Qt::SolidPattern));
	this->mapScene = new QGraphicsScene(ui->mapView);
	this->tilesScene = new QGraphicsScene(ui->tilesetView);
	ui->layersTable->setColumnWidth(0,48);
	ui->layersTable->setColumnWidth(2,24);
	ui->layersTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
	ui->layersTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
	ui->layersTable->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);
	ui->mainSplitter->setStretchFactor(0,1);
}

MapEditor::~MapEditor() {
	delete ui;
	delete this->mapFile;
	delete this->mapScene;
	disconnect(this->pencilMenu, SIGNAL(triggered(QAction*)), this, SLOT(setPencilSize(QAction*)));
}

bool MapEditor::openFile(QString filename) {
	if(!this->mapFile->open(filename)) return false;
	return this->attach(this->mapFile);
}

bool MapEditor::attach(MapFile* attachedMap) {
	QList<MapFile::layer> layers = attachedMap->getLayers();
	int numLayers = layers.length();
	ui->layersTable->clear();
	ui->layersTable->setRowCount(numLayers);
	for(int l = numLayers; l > 0; l--) {
		int i = numLayers - l;
		MapFile::layer cur_layer = layers.at(i);

		QLabel* eyeLabel = new QLabel();
		eyeLabel->setToolTip("Toggle layer visibility");
		eyeLabel->setPixmap(QPixmap(":/icons/eye.png"));
		eyeLabel->setAlignment(Qt::AlignCenter);
		ui->layersTable->setCellWidget(l-1,0,eyeLabel);

		ui->layersTable->setItem(l-1,1, new QTableWidgetItem(cur_layer.name));

		QLabel* deleteLabel = new QLabel("X");
		deleteLabel->setToolTip("Delete layer");
		deleteLabel->setStyleSheet("color:red;font-weight:bold;");
		deleteLabel->setAlignment(Qt::AlignCenter);
		ui->layersTable->setCellWidget(l-1,2,deleteLabel);

		for(int t = 0; t < cur_layer.tiles.length(); t++) {
			uint16_t cur_tile = cur_layer.tiles.at(t);
			QGraphicsPixmapItem* tilePixmap = this->mapScene->addPixmap(QPixmap::fromImage(attachedMap->tileset->getImage(cur_tile)));

			int x = t % cur_layer.header.width;
			int y = (t - x) / cur_layer.header.width;
			tilePixmap->setOffset(x * attachedMap->tileset->header.tile_width,y * attachedMap->tileset->header.tile_height);
		}
	}
	ui->mapView->setScene(this->mapScene);
	QList<QImage> images = attachedMap->tileset->getTileImages();

	for(int i = 0; i < images.length(); i++) {
		QGraphicsPixmapItem* tilePixmap = this->tilesScene->addPixmap(QPixmap::fromImage(images.at(i)));
		tilePixmap->setOffset(i*attachedMap->tileset->header.tile_width+16,10)		;
	}
	ui->tilesetView->setScene(this->tilesScene);
	ui->tilesetBox->setTitle("Tiles (" + QString::number(attachedMap->tileset->header.num_tiles) + ")");
	ui->layersTable->selectRow(0);
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

void MapEditor::setPencilSize(QAction *size) {
	QString actionText = size->text();
	if(actionText == "1x1") {
		this->pencilSize = 1;
	} else if(actionText == "3x3") {
		this->pencilSize = 3;
	} else if(actionText == "5x5") {
		this->pencilSize = 5;
	}
	this->pencilMenu->setDefaultAction(size);
}
