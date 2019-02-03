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

#include "widgets/map/mapeditor.h"
#include "ui_mapeditor.h"
#include "widgets/sphereeditor.h"
#include "dialogs/layerpropertiesdialog.h"
#include "formats/mapfile.h"
#include "formats/tileset.h"
#include "util.h"

MapEditor::MapEditor(QWidget *parent) : SphereEditor(parent), ui(new Ui::MapEditor) {
	ui->setupUi(this);
	m_menuBar = new QToolBar();
//	m_menuBar->setFixedHeight(32);
	QActionGroup toolActions(m_menuBar);
	ui->layersTable->setContextMenuPolicy(Qt::CustomContextMenu);
	m_layerMenu = new QMenu(this);
	m_layerMenu->addAction("Insert layer");
	m_layerMenu->addAction("Delete layer");
	m_layerMenu->addAction("Duplicate layer");
	m_layerMenu->addSeparator();
	m_layerMenu->addAction("Toggle lock layer");
	m_layerMenu->addAction("Properties", this, SLOT(layerPropertiesRequested(bool)));

	m_pencilMenu = new QMenu(this);
	m_pencilMenu->addActions(QList<QAction*>({
		new QAction(QIcon(":/icons/1x1grid.png"), "1x1"),
		new QAction(QIcon(":/icons/3x3grid.png"), "3x3"),
		new QAction(QIcon(":/icons/5x5grid.png"), "5x5")
	}));
	m_pencilMenu->setDefaultAction(m_pencilMenu->actions().at(0));

	QToolButton* pencilMenuButton = new QToolButton();
	pencilMenuButton->setText("Pencil");
	pencilMenuButton->setIcon(QIcon(":/icons/pencil.png"));
	pencilMenuButton->setMenu(m_pencilMenu);
	pencilMenuButton->setPopupMode(QToolButton::MenuButtonPopup);
	m_menuBar->addWidget(pencilMenuButton);
	connect(m_pencilMenu, SIGNAL(triggered(QAction*)), this, SLOT(setPencilSize(QAction*)));

	m_menuBar->addAction(QIcon(":/icons/linetool.png"), "Line");
	m_menuBar->addAction(QIcon(":/icons/rectangletool.png"), "Rectangle");
	m_menuBar->addAction(QIcon(":/icons/paintbucket.png"), "Fill layer");
	m_menuBar->addAction(QIcon(":/icons/dropper.png"), "Select tile");
	connect(m_menuBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(setCurrentTool(QAction*)));

	m_mapView = new MapView(ui->mapViewLayout->widget());
	ui->mapViewLayout->addWidget(m_mapView);
	ui->mapViewLayout->setMenuBar(m_menuBar);
	m_mapView->setAlignment(Qt::AlignLeft|Qt::AlignTop);
	m_mapView->setBackgroundBrush(QBrush(Qt::darkGray, Qt::SolidPattern));

	m_tilesetView = new WrappedGraphicsView();
	m_tilesetLayout = new QVBoxLayout();
	m_tilesetLayout->addWidget(m_tilesetView);
	m_tilesetLayout->setMargin(0);
	ui->tilesetBox->setLayout(m_tilesetLayout);
	connect(m_tilesetView, SIGNAL(indexChanged(int)), this, SLOT(setTileIndex(int)));

	ui->layersTable->setColumnWidth(0,48);
	ui->layersTable->setColumnWidth(2,24);
	ui->layersTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
	ui->entitiesTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
	ui->mainSplitter->setStretchFactor(0,1);
}

MapEditor::~MapEditor() {
	disconnect(m_menuBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(setCurrentTool(QAction*)));
	disconnect(m_pencilMenu, SIGNAL(triggered(QAction*)), this, SLOT(setPencilSize(QAction*)));
	disconnect(m_tilesetView, SIGNAL(indexChanged(int)), this, SLOT(setTileIndex(int)));
	disconnect(m_layerMenu, SIGNAL(triggered(bool)), this, SLOT(layerPropertiesRequested(bool)));
	delete ui;
	delete m_mapView;
	delete m_pencilMenu;
	delete m_layerMenu;
	delete m_menuBar;
	delete m_tilesetView;
}

bool MapEditor::openFile(QString filename) {
	if(!m_mapView->openFile(filename)) return false;
	return attach(m_mapView->attachedMap());
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

	int numTiles = attachedMap->getTileset()->numTiles();
	Tileset* mapTileset = attachedMap->getTileset();

	for(int i = 0; i < numTiles; i++) {
		m_tilesetView->addPixmap(QPixmap::fromImage(mapTileset->getImage(i)));
	}

	ui->tilesetBox->setTitle("Tiles (" + QString::number(mapTileset->numTiles()) + ")");
	ui->layersTable->selectRow(0);
	ui->mainSplitter->setSizes(QList<int>({width()-300, 300}));
	ui->tilesSplitter->setSizes(QList<int>({1,1}));
	return true;
}

void MapEditor::on_layersTable_cellClicked(int row, int column) {
	switch(column) {
		case 0: {
			QWidget* item = ui->layersTable->cellWidget(row,column);
			QLabel* eyeLabel = dynamic_cast<QLabel*>(item);
			if(m_mapView->toggleLayerVisible(ui->layersTable->rowCount()-row-1))
				eyeLabel->setPixmap(QPixmap(":/icons/eye.png"));
			else
				eyeLabel->setPixmap(QPixmap(":/icons/eye-closed.png"));
		}
		break;
		case 2: {
			if(ui->layersTable->rowCount() > 1)
				ui->layersTable->removeRow(row);
		}
		break;
	}
}

void MapEditor::setPencilSize(QAction *size) {
	QString actionText = size->text();
	if(actionText == "1x1") {
		m_mapView->setDrawSize(1);
	} else if(actionText == "3x3") {
		m_mapView->setDrawSize(3);
	} else if(actionText == "5x5") {
		m_mapView->setDrawSize(5);
	}
	m_pencilMenu->setDefaultAction(size);
}

void MapEditor::setCurrentTool(QAction* tool) {
	//	qDebug() << tool->text();
}

void MapEditor::setTileIndex(int tile) {
	m_mapView->setCurrentTile(tile);
}

void MapEditor::on_layersTable_customContextMenuRequested(const QPoint &pos) {
	m_layerMenu->exec(ui->layersTable->mapToGlobal(pos));
}

void MapEditor::layerPropertiesRequested(bool triggered) {
	LayerPropertiesDialog lpd;
	MapFile::layer* l = m_mapView->getLayer(ui->layersTable->currentRow());
	lpd.setName(l->name);
	lpd.setSize(l->header.width,l->header.height);
	lpd.setReflectiveEnabled(l->header.reflective);
	lpd.setParallax(l->header.parallax_x,l->header.parallax_y);
	lpd.setAutoScrolling(l->header.scrolling_x,l->header.scrolling_y);
	if(lpd.exec() == QDialog::Rejected) return;

	ui->layersTable->item(ui->layersTable->currentRow(),1)->setText(lpd.getName());
}

void MapEditor::on_layersTable_itemChanged(QTableWidgetItem *item) {
	if(item->column() != 1) return;
	m_mapView->getLayer(item->row())->name = item->text();
}
