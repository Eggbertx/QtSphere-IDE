#include <QActionGroup>
#include <QDir>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTableWidgetItem>
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
	m_pencil1 = m_pencilMenu->addAction(QIcon(":/icons/1x1grid.png"), "1x1");
	m_pencil3 = m_pencilMenu->addAction(QIcon(":/icons/3x3grid.png"), "3x3");
	m_pencil5 = m_pencilMenu->addAction(QIcon(":/icons/5x5grid.png"), "5x5");
	m_pencilMenu->setDefaultAction(m_pencil1);
	connect(m_pencilMenu, SIGNAL(triggered(QAction*)), this, SLOT(setPencilSize(QAction*)));

	m_pencilTool = m_menuBar->addAction(QIcon(":/icons/pencil.png"), "Pencil");
	m_pencilTool->setMenu(m_pencilMenu);
	m_pencilTool->setCheckable(true);
	m_pencilTool->setChecked(true);
	m_lineTool = m_menuBar->addAction(QIcon(":/icons/linetool.png"), "Line");
	m_lineTool->setCheckable(true);
	m_rectTool = m_menuBar->addAction(QIcon(":/icons/rectangletool.png"), "Rectangle");
	m_rectTool->setCheckable(true);
	m_fillTool = m_menuBar->addAction(QIcon(":/icons/paintbucket.png"), "Fill layer");
	m_fillTool->setCheckable(true);
	m_dropperTool = m_menuBar->addAction(QIcon(":/icons/dropper.png"), "Select tile");
	m_dropperTool->setCheckable(true);
	m_menuBar->addSeparator();
	m_gridTool = m_menuBar->addAction(QIcon(":/icons/togglegrid.png"), "Show/Hide grid");
	m_gridTool->setCheckable(true);
	connect(m_menuBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(setCurrentTool(QAction*)));


	ui->mapViewLayout->setMenuBar(m_menuBar);
	ui->mapView->setBackgroundBrush(QBrush(Qt::darkGray, Qt::SolidPattern));

	connect(ui->tilesetView, SIGNAL(indexChanged(int)), this, SLOT(setTileIndex(int)));

	ui->layersTable->setColumnWidth(0,48);
	ui->layersTable->setColumnWidth(2,24);
	ui->layersTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
	ui->entitiesTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
	ui->mainSplitter->setStretchFactor(0,1);
}

MapEditor::~MapEditor() {
	disconnect(m_menuBar, SIGNAL(actionTriggered(QAction*)), this, SLOT(setCurrentTool(QAction*)));
	disconnect(m_pencilMenu, SIGNAL(triggered(QAction*)), this, SLOT(setPencilSize(QAction*)));
	disconnect(ui->tilesetView, SIGNAL(indexChanged(int)), this, SLOT(setTileIndex(int)));
	disconnect(this, SLOT(layerPropertiesRequested(bool)));
	delete ui;
	delete m_pencilMenu;
	delete m_layerMenu;
	delete m_menuBar;
}

bool MapEditor::openFile(QString filename) {
	if(!ui->mapView->openFile(filename)) return false;
	else attach(ui->mapView->attachedMap());
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
		if(attachedMap->isLayerVisible(i))
			eyeLabel->setPixmap(QPixmap(":/icons/eye.png"));
		else
			eyeLabel->setPixmap(QPixmap(":/icons/eye-closed.png"));
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
		ui->tilesetView->addPixmap(QPixmap::fromImage(mapTileset->getImage(i)));

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
			if(ui->mapView->toggleLayerVisible(ui->layersTable->rowCount()-row-1))
				eyeLabel->setPixmap(QPixmap(":/icons/eye.png"));
			else
				eyeLabel->setPixmap(QPixmap(":/icons/eye-closed.png"));
		}
		break;
		case 2: {
			if(ui->layersTable->rowCount() > 1) {
				ui->mapView->deleteLayer(ui->layersTable->rowCount()-row-1);
				ui->layersTable->removeRow(row);
			}
		}
		break;
	}
}

void MapEditor::setPencilSize(QAction *size) {
	qDebug("text: %s\n", size->text().toStdString().c_str());
	if(size == m_pencil1) ui->mapView->setDrawSize(1);
	else if(size == m_pencil3) ui->mapView->setDrawSize(3);
	else if(size == m_pencil5) ui->mapView->setDrawSize(5);

	m_pencilMenu->setDefaultAction(size);
}

void MapEditor::setCurrentTool(QAction* tool) {
	if(tool == m_pencil1 || tool == m_pencil3 || tool == m_pencil5) return;
	m_pencilTool->setChecked(false);
	m_lineTool->setChecked(false);
	m_rectTool->setChecked(false);
	m_fillTool->setChecked(false);
	m_dropperTool->setChecked(false);

	if(tool == m_gridTool) {
		ui->mapView->setGridVisible(m_gridTool->isChecked());
		return;
	}
	if(tool == m_pencilTool) {
		ui->mapView->setCurrentTool(MapEditor::Pencil);
		m_pencilTool->setChecked(true);
	} else if(tool == m_lineTool) {
		ui->mapView->setCurrentTool(MapEditor::Line);
		m_lineTool->setChecked(true);
	} else if(tool == m_rectTool) {
		ui->mapView->setCurrentTool(MapEditor::Rectangle);
		m_rectTool->setChecked(true);
	} else if(tool == m_fillTool) {
		ui->mapView->setCurrentTool(MapEditor::Fill);
		m_fillTool->setChecked(true);
	} else if(tool == m_dropperTool) {
		ui->mapView->setCurrentTool(MapEditor::Select);
		m_dropperTool->setChecked(true);
	}
}

void MapEditor::setTileIndex(int tile) {
	ui->mapView->setCurrentTile(tile);
}

void MapEditor::on_layersTable_customContextMenuRequested(const QPoint &pos) {
	m_layerMenu->exec(ui->layersTable->mapToGlobal(pos));
}

void MapEditor::layerPropertiesRequested(bool triggered) {
	LayerPropertiesDialog lpd;
	MapFile::layer* l = ui->mapView->getLayer(ui->layersTable->currentRow());
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
	ui->mapView->getLayer(item->row())->name = item->text();
}

void MapEditor::on_layersTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn) {
	ui->mapView->setCurrentLayer(ui->layersTable->rowCount()-currentRow-1);
}

void MapEditor::createUndoActions() {

}
