#include <QEvent>
#include <QGraphicsPixmapItem>
#include <QResizeEvent>
#include <QSettings>

#include "widgets/wrappedgraphicsview.h"

WrappedGraphicsView::WrappedGraphicsView(QWidget *parent): QGraphicsView(parent) {
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setAlignment(Qt::AlignTop|Qt::AlignLeft);
	setBackgroundBrush(Qt::darkGray);
	m_selectedIndex = 0;
	m_wScene = new QGraphicsScene(this);
	setScene(m_wScene);
	m_tSize = QSize(0,0);
	m_scaleMult = 1;
}

void WrappedGraphicsView::resizeEvent(QResizeEvent* resize) {
    arrangeItems();
	resize->accept();
}

void WrappedGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
	int newIndex = indexAt(event->pos());
	if(newIndex > -1) {
		if(m_selectedIndex != newIndex) emit indexChanged(newIndex);
		m_selectedIndex = newIndex;
		arrangeItems();
	}
}

int WrappedGraphicsView::indexAt(QPoint pos) {
	int x = 0;
	int y = 0;
	int numPixmaps = m_pixmaps.length();
	for(int i = 0; i < numPixmaps; i++) {
		QSize size = m_pixmaps.at(i).size();
		if(x + size.width() > width()) {
			x = 0;
			y += size.height();
		}
		if(QRect(QPoint(x,y),size).contains(pos)) return i;
		x += size.width();
	}

	return -1;
}

void WrappedGraphicsView::addPixmap(QPixmap pixmap) {
	m_pixmaps.append(pixmap);
    //arrangeItems();
}

QSize WrappedGraphicsView::tileSize() {
	return m_tSize;
}

void WrappedGraphicsView::setScaleFactor(int factor) {
	if(factor < 1) return;

	int oldFactor = m_scaleMult;
	m_scaleMult = factor;
	if(oldFactor != m_scaleMult) arrangeItems(width(),height());
}

int WrappedGraphicsView::scaleFactor() {
	return m_scaleMult;
}

int WrappedGraphicsView::arrangeItems(int width, int height) {
	if(m_pixmaps.length() == 0) return 0;
	QSettings settings;
	QColor cursorColor(settings.value("mapCursorColor","#0080FF").toString());
	if(!cursorColor.isValid()) cursorColor = QColor(0,128,255,128);
	cursorColor.setAlpha(128);
	int x = 0;
	int y = 0;
	int rows = 0;
	if(width == -1 && height == -1) {
		m_tSize = m_pixmaps.at(0).size();
	} else {
//		m_tSize = scene()->items().at(0)->boundingRect().size().toSize();
		m_tSize = m_wScene->items().at(0)->boundingRect().size().toSize();
	}

	int numItems = m_pixmaps.length();
    qDebug("numItems: %d\n", numItems);
	m_wScene->clear();

	for(int i = 0; i < numItems; i++) {
		QGraphicsPixmapItem* pixmap = m_wScene->addPixmap(m_pixmaps.at(i));
		pixmap->setScale(m_scaleMult);
		if(x + m_tSize.width() > this->width()) {
			x = 0;
			y += m_tSize.height();
		}

		pixmap->setPos(x*m_scaleMult,y*m_scaleMult);
		if(m_selectedIndex == i) {
			QGraphicsRectItem* selectionBox = m_wScene->addRect(x,y,m_tSize.width()-1, m_tSize.height()-1);
			selectionBox->setBrush(cursorColor);
			selectionBox->setZValue(1);
		}
		setSceneRect(0,0,x+m_tSize.width()*m_scaleMult,y+m_tSize.height()*m_scaleMult);
		x += m_tSize.width();
	}
	return rows;
}
