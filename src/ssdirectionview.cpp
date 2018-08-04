#include <QEvent>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QLayout>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QStyle>
#include <QToolButton>

#include "objects/spriteset.h"
#include "spritesetview.h"
#include "ssdirectionview.h"
#include "util.h"

SSDirectionView::SSDirectionView(QString name, int numFrames, Spriteset::SSDirection* ssDirection, QWidget *parent): QFrame(parent) {
	this->zoomFactor = 2;
	this->ssDirection = ssDirection;
	if(parent != nullptr) {
		this->spriteset = static_cast<SpritesetView*>(parent)->spriteset;
	}
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(showContextMenu(const QPoint&)));

	QHBoxLayout* dirLayout = new QHBoxLayout();
	dirLayout->setSpacing(0);
	dirLayout->setMargin(0);
	this->setLayout(dirLayout);

	this->nameLineEdit = new QLineEdit(name);
	this->nameLineEdit->setText(name);
	this->nameLineEdit->setFixedWidth(100);
	connect(this->nameLineEdit, SIGNAL(textChanged(QString)), this,
			SLOT(changeDirectionName(QString)));
	dirLayout->addWidget(this->nameLineEdit);
	this->framesContainer = new QWidget();
	this->framesLayout = new QHBoxLayout();
	this->framesLayout->setSpacing(1);
	this->framesLayout->setMargin(2);
	this->framesContainer->setLayout(this->framesLayout);

	this->frameViews = QList<QGraphicsView*>();
	for(int f = 0; f < numFrames; f++) {
		this->addFrame(nullptr);
	}
	dirLayout->addWidget(this->framesContainer);

	this->removeFrameButton = new QToolButton(this);
	this->removeFrameButton->setText("-");
	connect(this->removeFrameButton, SIGNAL(clicked(bool)), this, SLOT(removeFrameSlot()));
	dirLayout->addWidget(this->removeFrameButton);

	this->addFrameButton = new QToolButton(this);
	this->addFrameButton->setText("+");
	connect(this->addFrameButton, SIGNAL(clicked(bool)), this, SLOT(addFrameSlot()));
	dirLayout->addWidget(addFrameButton);
	dirLayout->addStretch(5);
}

SSDirectionView::~SSDirectionView() {
	disconnect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			   this, SLOT(showContextMenu(const QPoint&)));
	disconnect(this->removeFrameButton, SIGNAL(triggered(QAction*)),
			   this, SLOT(removeFrameSlot()));
	disconnect(this->addFrameButton, SIGNAL(triggered(QAction*)),
			   this, SLOT(addFrameSlot()));
}

void SSDirectionView::setImage(int index, QImage image) {
	this->frameViews.at(index)->setFixedSize(image.width()*this->zoomFactor+this->zoomFactor, image.height()*this->zoomFactor+this->zoomFactor);
	QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));

	if(item == nullptr) {
		QImage first = this->spriteset->images.first();
		item = new QGraphicsPixmapItem(QPixmap::fromImage(first));
	}
	item->setScale(this->zoomFactor);
	this->frameViews.at(index)->scene()->addItem(item);
}

QString SSDirectionView::getName() {
	return this->nameLineEdit->text();
}

void SSDirectionView::setName(QString name) {
	this->nameLineEdit->setText(name);
}

void SSDirectionView::showContextMenu(const QPoint &pos) {
	QMenu contextMenu(this);
	contextMenu.addAction("Insert");
	contextMenu.addAction("Append");
	contextMenu.addAction("Delete");
	contextMenu.addSeparator();
	contextMenu.addAction("Insert frames...");
	contextMenu.addAction("Append frames...");
	contextMenu.addAction("Delete frames...");
	contextMenu.addSeparator();
	contextMenu.addAction("Cut (Ctrl+X)");
	contextMenu.addAction("Copy (Ctrl+C)");
	contextMenu.addAction("Paste (Ctrl+V)");
	contextMenu.addSeparator();
	contextMenu.addAction("Insert from image");
	contextMenu.addAction("Append from image");

	QAction* result = contextMenu.exec(this->mapToGlobal(pos));
	if(!result) return;
	QString text = result->text();
	if(text == "Insert") {

	} else if(text == "Append") {

	}
}

void SSDirectionView::removeFrame(int index) {
	delete this->frameViews.takeAt(index);
}

int SSDirectionView::getZoom() {
	return this->zoomFactor;
}

void SSDirectionView::setZoom(int factor) {
	this->zoomFactor = factor;
}

void SSDirectionView::addFrame(QImage* image) {
	/*QPushButton* btn = new QPushButton("");
	btn->setFlat(true);
	btn->setAutoFillBackground(true);
	QIcon icon(QPixmap::fromImage(this->spriteset->images.first()).scaled(32*2,32*2));
	btn->setIconSize(btn->size());
	btn->setIcon(icon);
	btn->setStyleSheet("border:4px solid white");*/

	QGraphicsView* newView = new QGraphicsView(new QGraphicsScene());
	newView->setObjectName("frame" + QString::number(this->frameViews.length()));
	newView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	newView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	int scaledWidth = 32*this->zoomFactor;
	int scaledHeight = 32*this->zoomFactor;
	//btn->setFixedSize(scaledWidth, scaledHeight);
	newView->setBackgroundBrush(QBrush(QPixmap(":/icons/transparency-bg.png")));

	QGraphicsPixmapItem* frameImage;
	if(image == nullptr) {
		frameImage = new QGraphicsPixmapItem(QPixmap::fromImage(this->spriteset->images.first()));
		scaledWidth = frameImage->pixmap().width()*this->zoomFactor;
		scaledHeight = frameImage->pixmap().height()*this->zoomFactor;
	} else {
		frameImage = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
		scaledWidth = image->width()*this->zoomFactor;
		scaledHeight = image->height()*this->zoomFactor;
	}

	newView->scene()->addItem(frameImage);
	frameImage->setScale(this->zoomFactor);
	newView->setFixedSize(scaledWidth,scaledHeight);

	//this->framesLayout->addWidget(btn);
	this->framesLayout->addWidget(newView);
	this->frameViews.append(newView);
}

void SSDirectionView::removeFrameSlot() {
	if(this->frameViews.length() > 1)
		this->removeFrame(this->frameViews.length()-1);
}

void SSDirectionView::addFrameSlot() {
	this->addFrame(&this->spriteset->images.first());
}

void SSDirectionView::changeDirectionName(QString name) {
	this->ssDirection->name = name;
}

void SSDirectionView::mousePressEvent(QMouseEvent* event) {
	if(event->button() == Qt::LeftButton) {
		// QWidget* child = this->childAt(event->pos());
		// qDebug() << child->objectName();
	}
	event->accept();
}
