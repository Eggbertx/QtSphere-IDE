#include <QGraphicsView>
#include <QBrush>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPixmap>
#include <QTimer>

#include "animatablepixmap.h"

AnimatablePixmap::AnimatablePixmap(bool showTransparency, QWidget *parent) : QGraphicsView(parent) {
	this->init(showTransparency);

}

AnimatablePixmap::AnimatablePixmap(QPixmap* singleImage, bool showTransparency, QWidget* parent) : QGraphicsView(parent) {
	this->init(showTransparency);
	PixmapFrame* frame = new PixmapFrame();
	frame->pixmap = singleImage;
	frame->delay = 1;
	this->frames << frame;
	this->timer = new QTimer(this);
	QGraphicsPixmapItem* item = new QGraphicsPixmapItem(*singleImage);

}
void AnimatablePixmap::init(bool showTransparency) {
	this->showTransparency = showTransparency;
	this->setBackgroundBrush(QBrush(QPixmap(":/icons/transparency-bg.png")));
	this->setScene(new QGraphicsScene(this));
}

void AnimatablePixmap::setFrames(QList<PixmapFrame*> frames) {
	this->frames = frames;
}

int AnimatablePixmap::nextFrame() {
	return -1;
}

QPixmap* AnimatablePixmap::getImage(int index) {
	return const_cast<QPixmap*>(this->frames.at(index)->pixmap);
}

int AnimatablePixmap::getDelay(int index) {
	return this->frames.at(index)->delay;
}

void AnimatablePixmap::setImage(int index, QPixmap* pixmap) {
	if(this->frames.at(index) == nullptr) return;
	this->frames.at(index)->pixmap = pixmap;
}

void AnimatablePixmap::setDelay(int index, int delay) {
	if(this->frames.at(index) == nullptr) return;
	this->frames.at(index)->delay = delay;
}

void AnimatablePixmap::setFrame(int index, PixmapFrame* frame) {
	if(this->frames.at(index) == nullptr) return;
	this->frames.replace(index, frame);
}
