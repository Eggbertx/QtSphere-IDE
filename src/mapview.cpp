#include <QDebug>
#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QScrollBar>
#include <QPen>
#include <math.h>

#include "formats/mapfile.h"
#include "mainwindow.h"
#include "mapview.h"

MapView::MapView(QWidget *parent) : QGraphicsView(parent) {
	this->mapStatusFormat = "Map tile: (%1, %2) Pixel: (%3, %4)";
	this->mapScene = new QGraphicsScene(this);
	this->setMouseTracking(true);
	this->mapFile = new MapFile(this);
	this->setScene(this->mapScene);
	this->pointerGroup = new QGraphicsItemGroup();
	this->mapScene->addItem(this->pointerGroup);
}

MapView::~MapView() {
	delete this->mapFile;
	delete this->mapScene;
}

bool MapView::attachMap(MapFile* map) {
	QList<MapFile::layer> layers = this->mapFile->getLayers();
	int numLayers = layers.length();
	for(int l = numLayers; l > 0; l--) {
		int i = numLayers - l;
		MapFile::layer cur_layer = layers.at(i);

		for(int t = 0; t < cur_layer.tiles.length(); t++) {
			uint16_t cur_tile = cur_layer.tiles.at(t);
			QGraphicsPixmapItem* tilePixmap = this->mapScene->addPixmap(QPixmap::fromImage(map->tileset->getImage(cur_tile)));

			int x = t % cur_layer.header.width;
			int y = (t - x) / cur_layer.header.width;
			tilePixmap->setPos(x * map->tileset->header.tile_width,y * map->tileset->header.tile_height);
		}
	}

	this->mapFile = map;
	this->setDrawSize(1);
	return true;
}

bool MapView::openFile(QString filename) {
	if(!this->mapFile->open(filename)) return false;
	return this->attachMap(this->mapFile);
}

MapFile *MapView::attachedMap() {
	return this->mapFile;
}

QPoint MapView::widgetToMapPos(QPoint pos) {
	return this->widgetToMapPos(pos.x(), pos.y());
}

QPoint MapView::widgetToMapPos(int x, int y) {
	if(!this->mapFile) return QPoint(-1, -1);
	QSize tileSize = this->mapFile->tileSize();
	return QPoint(floor(x/tileSize.width()), floor(y/tileSize.height()));
}

QPoint MapView::mapToWidgetPos(QPoint pos) {
	return this->widgetToMapPos(pos.x(), pos.y());
}

QPoint MapView::mapToWidgetPos(int x, int y) {
	if(!this->mapFile) return QPoint(-1, -1);
	QSize tileSize = this->mapFile->tileSize();
	return QPoint(x*tileSize.width(), y*tileSize.height());
}

void MapView::setDrawSize(int size) {
	this->createPointer(size);
	this->drawSize = size;
}

int MapView::getDrawSize() {
	return this->drawSize;
}

void MapView::setCurrentTile(int tile) {
	this->currentTile = tile;
}

MapFile::layer* MapView::getLayer(int index) {
	return this->mapFile->getLayer(index);
}

void MapView::drawTile(int index) {
	int tileWidth = this->mapFile->tileSize().width();
	int tileHeight = this->mapFile->tileSize().height();
	QRect pRect = this->pointerRect(true);

	for(int rY = pRect.y(); rY < pRect.bottom()+1; rY++) {
		for(int rX = pRect.x(); rX < pRect.right()+1; rX++) {
			int sceneX = rX * tileWidth;
			int sceneY = rY * tileHeight;
			QList<QGraphicsItem*> mouseItems = this->items(sceneX,sceneY);
			foreach(QGraphicsItem* item, mouseItems) {
				if(item->type() != QGraphicsPixmapItem::Type) continue;
				QGraphicsPixmapItem* tile = reinterpret_cast<QGraphicsPixmapItem*>(item);
				tile->setPixmap(QPixmap::fromImage(this->mapFile->tileset->getImage(index)));
				break;
			}
		}
	}
}

void MapView::mouseMoveEvent(QMouseEvent* event) {
	QRect mapRect = *this->mapFile->largestLayerRect();
	QSize tileSize = this->mapFile->tileSize();
	QRect widgetRect(0,0,
		mapRect.width() * tileSize.width(),
		mapRect.height() * tileSize.height()
	);

	if(widgetRect.contains(event->pos())) {
		this->pointerGroup->show();
	} else {
		MainWindow::instance()->setStatus("Ready");
		this->pointerGroup->hide();
		return;
	}

	QPoint tilePos = this->widgetToMapPos(
		event->pos().x() + this->horizontalScrollBar()->value()-1,
		event->pos().y() + this->verticalScrollBar()->value()-1
	);
	QPoint pointerUL = this->mapToWidgetPos(
		tilePos.x()-floor(this->drawSize/2),
		tilePos.y()-floor(this->drawSize/2)
	);
	MainWindow::instance()->setStatus(this->mapStatusFormat.arg(
		QString::number(tilePos.x()),QString::number(tilePos.y()),
		QString::number(event->pos().x()), QString::number(event->pos().y())
	));
	this->pointerGroup->setPos(pointerUL);

	if(this->drawing) {
		this->drawTile(this->currentTile);
	}
	this->setSceneRect(widgetRect);
}

void MapView::mousePressEvent(QMouseEvent* event) {
	this->drawing = true;
	this->drawTile(this->currentTile);
}

void MapView::mouseReleaseEvent(QMouseEvent *event) {
	this->drawing = false;
}

void MapView::leaveEvent(QEvent *event) {
	this->pointerGroup->hide();
	this->drawing = false;
	MainWindow::instance()->setStatus("Ready");
}

QGraphicsItemGroup* MapView::createPointer(int size) {
	this->mapScene->removeItem(this->pointerGroup);
	QGraphicsItemGroup* pointerGroup = new QGraphicsItemGroup();
	QSize rectSize = this->mapFile->tileSize();
	for(int y = 0; y < size; y++) {
		for(int x = 0; x < size; x++) {
			QGraphicsRectItem* ri = new QGraphicsRectItem(x*rectSize.width(),y*rectSize.height(),rectSize.width(),rectSize.height());
			ri->setPen(QPen(Qt::yellow));
			pointerGroup->addToGroup(ri);
		}
	}
	this->pointerGroup = pointerGroup;
	this->mapScene->addItem(this->pointerGroup);
	return pointerGroup;
}

QRect MapView::pointerRect(bool tiles) {
	int rX = this->pointerGroup->x();
	int rY = this->pointerGroup->y();
	if(tiles) {
		QPoint tilePos = this->widgetToMapPos(
			rX + this->horizontalScrollBar()->value(),
			rY + this->verticalScrollBar()->value()
		);
		rX = tilePos.x();
		rY = tilePos.y();
	}

	QSize rSize = QSize(this->drawSize, this->drawSize);
	if(!tiles) {
		rSize.setWidth(rSize.width() * this->mapFile->tileSize().width());
		rSize.setHeight(rSize.height() * this->mapFile->tileSize().height());
	}
	return QRect(rX, rY, rSize.width(), rSize.height());
}
