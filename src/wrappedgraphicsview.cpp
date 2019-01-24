#include <QDebug>
#include <QEvent>
#include <QGraphicsPixmapItem>
#include <QResizeEvent>

#include "wrappedgraphicsview.h"

WrappedGraphicsView::WrappedGraphicsView(QWidget *parent): QGraphicsView(parent) {
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setAlignment(Qt::AlignTop|Qt::AlignLeft);
	this->setBackgroundBrush(Qt::darkGray);
	this->selectedIndex = 0;
	this->wScene = new QGraphicsScene(this);
	this->setScene(this->wScene);
	this->tSize = QSize(0,0);
	this->scaleMult = 1;
}

void WrappedGraphicsView::resizeEvent(QResizeEvent* resize) {
	this->arrangeItems();
	resize->accept();
}

void WrappedGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
	int newIndex = this->indexAt(event->pos());
	if(newIndex > -1) {
		this->selectedIndex = newIndex;
		this->arrangeItems();
	}
}

int WrappedGraphicsView::indexAt(QPoint pos) {
	int x = 0;
	int y = 0;
	int numPixmaps = this->pixmaps.length();
	for(int i = 0; i < numPixmaps; i++) {
		QSize size = this->pixmaps.at(i).size();
		if(x + size.width() > this->width()) {
			x = 0;
			y += size.height();
		}
		if(QRect(QPoint(x,y),size).contains(pos)) return i;
		x += size.width();
	}

	return -1;
}

void WrappedGraphicsView::addPixmap(QPixmap pixmap) {
	this->pixmaps.append(pixmap);
	this->arrangeItems();
}

QSize WrappedGraphicsView::tileSize() {
	return this->tSize;
}

void WrappedGraphicsView::setScaleFactor(int factor) {
	if(factor < 1) return;

	int oldFactor = this->scaleMult;
	this->scaleMult = factor;
	if(oldFactor != this->scaleMult) this->arrangeItems(this->width(),this->height());
}

int WrappedGraphicsView::scaleFactor() {
	return this->scaleMult;
}

int WrappedGraphicsView::arrangeItems(int width, int height) {
	if(this->pixmaps.length() == 0) return 0;
	int x = 0;
	int y = 0;
	int rows = 0;
	if(width == -1 && height == -1) {
		this->tSize = this->pixmaps.at(0).size();
	} else {
		this->tSize = this->scene()->items().at(0)->boundingRect().size().toSize();
	}


	int numItems = this->pixmaps.length();
	this->wScene->clear();

	for(int i = 0; i < numItems; i++) {
		QGraphicsPixmapItem* pixmap = this->wScene->addPixmap(this->pixmaps.at(i));
		pixmap->setScale(this->scaleMult);
		if(x + tSize.width() > this->width()) {
			x = 0;
			y += tSize.height();
		}

		pixmap->setPos(x*this->scaleMult,y*this->scaleMult);
		if(this->selectedIndex == i) {
			QGraphicsRectItem* selectionBox = this->wScene->addRect(x,y,tSize.width()-1, tSize.height()-1, QPen(Qt::yellow,1));
			selectionBox->setZValue(1);
		}
		this->setSceneRect(0,0,x+tSize.width()*this->scaleMult,y+tSize.height()*this->scaleMult);
		x += tSize.width();
	}
	return rows;
}
