#include <QActionGroup>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QToolBar>
#include <QToolButton>
#include <QWidget>

#include "mapeditor.h"
#include "ui_mapeditor.h"
#include "sphereeditor.h"
#include "formats/mapfile.h"
#include "formats/tileset.h"
#include "util.h"

MapEditor::MapEditor(QWidget *parent) : SphereEditor(parent), ui(new Ui::MapEditor) {
	ui->setupUi(this);
	this->menuBar = new QToolBar();
//	this->menuBar->setFixedHeight(32);
	QActionGroup toolActions(this->menuBar);

	this->pencilMenu = new QMenu(this);
	this->pencilMenu->addActions(QList<QAction*>({
		new QAction(QIcon(":/icons/1x1grid.png"), "1x1"),
		new QAction(QIcon(":/icons/3x3grid.png"), "3x3"),
		new QAction(QIcon(":/icons/5x5grid.png"), "5x5")
	}));
	this->pencilMenu->setDefaultAction(this->pencilMenu->actions().at(0));

	QToolButton* pencilMenuButton = new QToolButton();
	pencilMenuButton->setText("Pencil");
	pencilMenuButton->setIcon(QIcon(":/icons/pencil.png"));
	pencilMenuButton->setMenu(this->pencilMenu);
	pencilMenuButton->setPopupMode(QToolButton::MenuButtonPopup);
	this->menuBar->addWidget(pencilMenuButton);
	connect(this->pencilMenu, SIGNAL(triggered(QAction*)), this, SLOT(setPencilSize(QAction*)));

	this->menuBar->addAction(QIcon(":/icons/linetool.png"), "Line");
	this->menuBar->addAction(QIcon(":/icons/rectangletool.png"), "Rectangle");
	this->menuBar->addAction(QIcon(":/icons/paintbucket.png"), "Fill layer");
	this->menuBar->addAction(QIcon(":/icons/dropper.png"), "Select tile");
	connect(this->menuBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(setCurrentTool(QAction*)));

	this->mapView = new MapView(ui->mapViewLayout->widget());
	ui->mapViewLayout->addWidget(this->mapView);
	ui->mapViewLayout->setMenuBar(this->menuBar);
	this->mapView->setAlignment(Qt::AlignLeft|Qt::AlignTop);
	this->mapView->setBackgroundBrush(QBrush(Qt::darkGray, Qt::SolidPattern));

	this->tilesetView = new WrappedGraphicsView();
	this->tilesetLayout = new QVBoxLayout();
	this->tilesetLayout->addWidget(tilesetView);
	this->tilesetLayout->setMargin(0);
	ui->tilesetBox->setLayout(tilesetLayout);
	connect(this->tilesetView, SIGNAL(indexChanged(int)), this, SLOT(setTileIndex(int)));

	ui->layersTable->setColumnWidth(0,48);
	ui->layersTable->setColumnWidth(2,24);
	ui->layersTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
	ui->entitiesTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
	ui->mainSplitter->setStretchFactor(0,1);
}

MapEditor::~MapEditor() {
	disconnect(this->menuBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(setCurrentTool(QAction*)));
	disconnect(this->pencilMenu, SIGNAL(triggered(QAction*)), this, SLOT(setPencilSize(QAction*)));
	disconnect(this->tilesetView, SIGNAL(indexChanged(int)), this, SLOT(setTileIndex(int)));
	delete ui;
	delete this->mapView;
	delete this->pencilMenu;
	delete this->menuBar;
}

bool MapEditor::openFile(QString filename) {
	if(!this->mapView->openFile(filename)) return false;
	return this->attach(this->mapView->attachedMap());
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
	}

	QList<MapFile::entity> entities = attachedMap->getEntities();
	int numEntities = entities.length();
	ui->entitiesTable->clear();
	ui->entitiesTable->setRowCount(numEntities);
	for(int e = 0; e < numEntities; e++) {
		MapFile::entity cur_entity = entities.at(e);
		ui->entitiesTable->setCellWidget(e, 0, new QLabel(cur_entity.name));
		ui->entitiesTable->setCellWidget(e, 1, new QLabel(cur_entity.spriteset));
		QToolButton* browseBtn = new QToolButton();
		browseBtn->setText("...");
		ui->entitiesTable->setCellWidget(e,2,browseBtn);
		ui->entitiesTable->setColumnWidth(2,browseBtn->width());
	}

	int numTiles = attachedMap->tileset->numTiles();
	for(int i = 0; i < numTiles; i++) {
		this->tilesetView->addPixmap(QPixmap::fromImage(attachedMap->tileset->getImage(i)));
	}

	ui->tilesetBox->setTitle("Tiles (" + QString::number(attachedMap->tileset->header.num_tiles) + ")");
	ui->layersTable->selectRow(0);
	ui->mainSplitter->setSizes(QList<int>({this->width()-300, 300}));
	ui->tilesSplitter->setSizes(QList<int>({1,1}));
	return true;
}

void MapEditor::on_layersTable_cellClicked(int row, int column) {
	switch(column) {
		case 0: {
			QWidget* item = ui->layersTable->cellWidget(row,column);
			QLabel* eyeLabel = dynamic_cast<QLabel*>(item);
			MapFile::layer* toggled_layer = this->mapView->getLayer(row);
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
		this->mapView->setDrawSize(1);
	} else if(actionText == "3x3") {
		this->mapView->setDrawSize(3);
	} else if(actionText == "5x5") {
		this->mapView->setDrawSize(5);
	}
	this->pencilMenu->setDefaultAction(size);
}

void MapEditor::setCurrentTool(QAction* tool) {
	//	qDebug() << tool->text();
}

void MapEditor::setTileIndex(int tile) {
	this->mapView->setCurrentTile(tile);
}
