#include <QGraphicsItemGroup>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QScrollBar>
#include <QPen>
#include <QSettings>
#include <math.h>

#include "formats/mapfile.h"
#include "mainwindow.h"
#include "widgets/map/mapview.h"

MapView::MapView(QWidget *parent) : QGraphicsView(parent) {
	m_mapStatusFormat = "Map tile: (%1, %2) Pixel: (%3, %4)";
	m_mapScene = new QGraphicsScene(this);
	setMouseTracking(true);
	m_mapFile = new MapFile(this);
	setScene(m_mapScene);
	m_pointerGroup = new QGraphicsItemGroup();
	m_mapScene->addItem(m_pointerGroup);
}

MapView::~MapView() {
	delete m_mapFile;
	delete m_mapScene;
}

bool MapView::attachMap(MapFile* map) {
	QList<MapFile::layer> layers = m_mapFile->getLayers();
	int numLayers = layers.length();
	Tileset* mapTileset = map->getTileset();
	QSize tileSize = mapTileset->getTileSize();
	for(int l = numLayers; l > 0; l--) {
		int i = numLayers - l;
		MapFile::layer cur_layer = layers.at(i);
		bool visible = m_mapFile->isLayerVisible(numLayers-l);
		for(int t = 0; t < cur_layer.tiles.length(); t++) {
			uint16_t cur_tile = cur_layer.tiles.at(t);
			QGraphicsPixmapItem* tilePixmap = m_mapScene->addPixmap(QPixmap::fromImage(mapTileset->getImage(cur_tile)));
			tilePixmap->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
			int x = t % cur_layer.header.width;
			int y = (t - x) / cur_layer.header.width;
			tilePixmap->setPos(x * tileSize.width(),y * tileSize.height());
			tilePixmap->setZValue(cur_layer.index);
			tilePixmap->setVisible(visible);
		}
	}

	m_mapFile = map;
	setDrawSize(1);
	return true;
}

bool MapView::openFile(QString filename) {
	if(!m_mapFile->open(filename)) return false;
	return attachMap(m_mapFile);
}

MapFile *MapView::attachedMap() {
	return m_mapFile;
}

QPoint MapView::widgetToMapPos(QPoint pos) {
	return widgetToMapPos(pos.x(), pos.y());
}

QPoint MapView::widgetToMapPos(int x, int y) {
	if(!m_mapFile) return QPoint(-1, -1);
	QSize tileSize = m_mapFile->getTileSize();
	return QPoint(floor(x/tileSize.width()), floor(y/tileSize.height()));
}

QPoint MapView::mapToWidgetPos(QPoint pos) {
	return widgetToMapPos(pos.x(), pos.y());
}

QPoint MapView::mapToWidgetPos(int x, int y) {
	if(!m_mapFile) return QPoint(-1, -1);
	QSize tileSize = m_mapFile->getTileSize();
	return QPoint(x*tileSize.width(), y*tileSize.height());
}

void MapView::setDrawSize(int size) {
	createPointer(size);
	m_drawSize = size;
}

int MapView::getDrawSize() {
	return m_drawSize;
}

void MapView::setCurrentTile(int tile) {
	m_currentTile = tile;
}

MapFile::layer* MapView::getLayer(int index) {
	return m_mapFile->getLayer(index);
}

void MapView::setLayerVisible(int layer, bool visible) {
	m_mapFile->setLayerVisible(layer, visible);
	foreach(QGraphicsItem* item, m_mapScene->items()) {
		if(item->zValue() == layer) item->setVisible(visible);
	}
}

void MapView::setCurrentLayer(int layer) {
	m_currentLayer = layer;
}

int MapView::getCurrentLayer() {
	return m_currentLayer;
}

bool MapView::toggleLayerVisible(int layer) {
	bool visible = m_mapFile->isLayerVisible(layer);
	m_mapFile->setLayerVisible(layer, !visible);
	foreach(QGraphicsItem* item, m_mapScene->items()) {
		if(item->zValue() == layer) item->setVisible(!visible);
	}
	return !visible;
}

void MapView::drawTile(int index) {
	int tileWidth = m_mapFile->getTileSize().width();
	int tileHeight = m_mapFile->getTileSize().height();
	QRect pRect = pointerRect(true);
	Tileset* mapTileset = m_mapFile->getTileset();

	for(int rY = pRect.y(); rY <= pRect.bottom(); rY++) {
		for(int rX = pRect.x(); rX <= pRect.right(); rX++) {
			int sceneX = rX * tileWidth;
			int sceneY = rY * tileHeight;
			QList<QGraphicsItem*> mouseItems = items(sceneX,sceneY);
			foreach(QGraphicsItem* item, mouseItems) {
				if(item->type() != QGraphicsPixmapItem::Type || item->zValue() != m_currentLayer) continue;
				QGraphicsPixmapItem* tile = reinterpret_cast<QGraphicsPixmapItem*>(item);
				tile->setPixmap(QPixmap::fromImage(mapTileset->getImage(index)));
				break;
			}
		}
	}
}

void MapView::mouseMoveEvent(QMouseEvent* event) {
	QRect mapRect = *m_mapFile->largestLayerRect();
	QSize tileSize = m_mapFile->getTileSize();
	QRect widgetRect(0,0,
		mapRect.width() * tileSize.width(),
		mapRect.height() * tileSize.height()
	);

	if(widgetRect.contains(event->pos())) {
		m_pointerGroup->show();
	} else {
		MainWindow::instance()->setStatus("Ready");
		m_pointerGroup->hide();
		return;
	}

	QPoint tilePos = widgetToMapPos(
		event->pos().x() + horizontalScrollBar()->value()-1,
		event->pos().y() + verticalScrollBar()->value()-1
	);
	QPoint pointerUL = mapToWidgetPos(
		tilePos.x()-floor(m_drawSize/2),
		tilePos.y()-floor(m_drawSize/2)
	);
	MainWindow::instance()->setStatus(m_mapStatusFormat.arg(
		QString::number(tilePos.x()),QString::number(tilePos.y()),
		QString::number(event->pos().x()), QString::number(event->pos().y())
	));
	m_pointerGroup->setPos(pointerUL);

	if(m_drawing) {
		drawTile(m_currentTile);
	}
	setSceneRect(widgetRect);
}

void MapView::mousePressEvent(QMouseEvent* event) {
	if(event->button() == Qt::LeftButton) {
		m_drawing = true;
		drawTile(m_currentTile);
	}
}

void MapView::mouseReleaseEvent(QMouseEvent *event) {
	m_drawing = false;
}

void MapView::leaveEvent(QEvent *event) {
	m_pointerGroup->hide();
	m_drawing = false;
	MainWindow::instance()->setStatus("Ready");
}

QGraphicsItemGroup* MapView::createPointer(int size) {
	QSettings settings;
	m_mapScene->removeItem(m_pointerGroup);
	QGraphicsItemGroup* pointerGroup = new QGraphicsItemGroup();
	QSize rectSize = m_mapFile->getTileSize();
	QColor cursorColor(settings.value("mapCursorColor","#0080FF").toString());
	if(!cursorColor.isValid()) cursorColor = QColor(0,128,255,128);

	cursorColor.setAlpha(128);
	for(int y = 0; y < size; y++) {
		for(int x = 0; x < size; x++) {
			QGraphicsRectItem* ri = new QGraphicsRectItem(x*rectSize.width(),y*rectSize.height(),rectSize.width(),rectSize.height());
			ri->setBrush(cursorColor);
			pointerGroup->addToGroup(ri);
		}
	}
	pointerGroup->setZValue(256);
	m_pointerGroup = pointerGroup;
	m_mapScene->addItem(m_pointerGroup);
	return pointerGroup;
}

QRect MapView::pointerRect(bool tiles) {
	int rX = m_pointerGroup->x();
	int rY = m_pointerGroup->y();
	if(tiles) {
		QPoint tilePos = widgetToMapPos(
			rX + horizontalScrollBar()->value(),
			rY + verticalScrollBar()->value()
		);
		rX = tilePos.x();
		rY = tilePos.y();
	}

	QSize rSize = QSize(m_drawSize, m_drawSize);
	if(!tiles) {
		rSize.setWidth(rSize.width() * m_mapFile->getTileSize().width());
		rSize.setHeight(rSize.height() * m_mapFile->getTileSize().height());
	}
	return QRect(rX, rY, rSize.width(), rSize.height());
}
