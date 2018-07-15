#include <QEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QList>
#include <QImage>
#include <QPoint>
#include <QPalette>
#include <QPixmap>
#include "imagechooser.h"
#include "objects/spriteset.h"

ImageChooser::ImageChooser(bool showTransparency, QWidget *parent): QWidget(parent) {
	this->installEventFilter(this);
	this->showTransparency = showTransparency;

	this->alphabg = QPixmap::fromImage(QImage(":/icons/transparency-bg.png"));
	this->images = QList<QImage>();
	QPalette pal = palette();
	pal.setColor(QPalette::Background, Qt::black);
	this->setAutoFillBackground(true);
	this->setPalette(pal);
	this->rightClickMenu = new QMenu(this);
	QMenu* fileMenu = this->rightClickMenu->addMenu("File");
	fileMenu->addAction("Save frame");
	fileMenu->addAction("Replace frame");
	fileMenu->addAction("Append from file");
	this->rightClickMenu->addAction("Append");
	this->rightClickMenu->addAction("Remove");
	this->rightClickMenu->addAction("Remove #");
}

ImageChooser::~ImageChooser() {

}

QList<QImage> ImageChooser::getImages() {
	return this->images;
}

void ImageChooser::setImages(QList<QImage> imglist) {
	this->images = imglist;
	this->repaint();
}

void ImageChooser::setImages(QImage imgarr[], int num) {
	this->images.clear();
	for(int i = 0; i < num; i++) {
		this->images << imgarr[i];
	}
}

bool ImageChooser::eventFilter(QObject* object, QEvent* event) {
	switch (event->type()) {
		case QEvent::Paint: {
			QPainter painter(this);
			//painter.drawTiledPixmap(all_rect, this->alphabg);
			painter.drawTiledPixmap(this->rect(), this->alphabg);
			int x = 0;
			int y = 0;
			for(int i = 0; i < this->images.length();i++) {
				if(x + this->images.at(i).width() > this->width()) {
					x = 0;
					y += this->images.at(i).height();
				}
				painter.drawImage(QPoint(x,y),this->images.at(i));
				if(i == this->selectedIndex) {
					painter.setPen(Qt::magenta);
					painter.drawRect(x,y,this->images.at(i).width(),this->images.at(i).height());
				}
				x += this->images.at(i).width();
			}
			break;
		}
		case QEvent::MouseMove: {
			QMouseEvent* me = static_cast<QMouseEvent*>(event);
			this->mousePos = me->pos();
			break;
		}
		case QEvent::MouseButtonPress: {
			QMouseEvent* me = static_cast<QMouseEvent*>(event);
			int buttonPressed = me->button();
			if(buttonPressed == Qt::LeftButton|Qt::RightButton) {
				int numImages = this->images.length();
				int x = 0;
				int y = 0;
				for(int i = 0; i < numImages; i++) {
					if(x + this->images.at(i).width() > this->width()) {
						x = 0;
						y += this->images.at(i).height();
					}
					if(x <= me->pos().x() && me->pos().x() < x + this->images.at(i).width() &&
						y <= me->pos().y() && me->pos().y() < y + this->images.at(i).height()) {
						this->selectedIndex = i;
						this->repaint();
					}
					x+= this->images.at(i).width();
				}
			}
			if(buttonPressed == Qt::RightButton) {
				this->rightClickMenu->exec(this->mapToGlobal(me->pos()));
			}
		}
		default:
			break;
	}
	(void)object;
	return true;
}
