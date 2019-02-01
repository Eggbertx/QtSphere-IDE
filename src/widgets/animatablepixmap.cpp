#include <QGraphicsView>
#include <QBrush>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPixmap>
#include <QTimer>

#include "widgets/animatablepixmap.h"

AnimatablePixmap::AnimatablePixmap(bool showTransparency, QWidget *parent) : QGraphicsView(parent) {
	init(showTransparency);

}

AnimatablePixmap::AnimatablePixmap(QPixmap* singleImage, bool showTransparency, QWidget* parent) : QGraphicsView(parent) {
	init(showTransparency);
	PixmapFrame* frame = new PixmapFrame();
	frame->pixmap = singleImage;
	frame->delay = 1;
	m_frames << frame;
	m_timer = new QTimer(this);
	QGraphicsPixmapItem* item = new QGraphicsPixmapItem(*singleImage);

}
void AnimatablePixmap::init(bool showTransparency) {
	m_showTransparency = showTransparency;
	setBackgroundBrush(QBrush(QPixmap(":/icons/transparency-bg.png")));
	setScene(new QGraphicsScene(this));
}

void AnimatablePixmap::setFrames(QList<PixmapFrame*> frames) {
	m_frames = frames;
}

int AnimatablePixmap::nextFrame() {
	return -1;
}

QPixmap* AnimatablePixmap::getImage(int index) {
	return const_cast<QPixmap*>(m_frames.at(index)->pixmap);
}

int AnimatablePixmap::getDelay(int index) {
	return m_frames.at(index)->delay;
}

void AnimatablePixmap::setImage(int index, QPixmap* pixmap) {
	if(m_frames.at(index) == nullptr) return;
	m_frames.at(index)->pixmap = pixmap;
}

void AnimatablePixmap::setDelay(int index, int delay) {
	if(m_frames.at(index) == nullptr) return;
	m_frames.at(index)->delay = delay;
}

void AnimatablePixmap::setFrame(int index, PixmapFrame* frame) {
	if(m_frames.at(index) == nullptr) return;
	m_frames.replace(index, frame);
}
