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

#include "formats/spriteset.h"
#include "widgets/spriteset/spriteseteditor.h"
#include "widgets/spriteset/ssdirectionview.h"
#include "util.h"

SSDirectionView::SSDirectionView(Spriteset* spriteset, int index, QWidget* parent): QFrame(parent) {
	m_spriteset = spriteset;
	m_directionIndex = index;
	m_direction = m_spriteset->getDirection(index);
	m_zoomFactor = 2;
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(showContextMenu(const QPoint&)));

	QHBoxLayout* dirLayout = new QHBoxLayout();
	dirLayout->setSpacing(0);
	dirLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(dirLayout);

	m_nameLineEdit = new QLineEdit();
	m_nameLineEdit->setText(m_direction->name);
	m_nameLineEdit->setFixedWidth(100);
	connect(m_nameLineEdit, SIGNAL(textChanged(QString)),
			this, SLOT(changeDirectionName(QString)));
	dirLayout->addWidget(m_nameLineEdit);
	m_framesContainer = new QWidget();
	m_framesLayout = new QHBoxLayout();
	m_framesLayout->setSpacing(1);
	m_framesLayout->setContentsMargins(2, 2, 2, 2);
	m_framesContainer->setLayout(m_framesLayout);

	m_frameViews = QList<QGraphicsView*>();
	int num_frames = m_direction->frames.length();
	for(int f = 0; f < num_frames; f++) {
		addFrame(m_spriteset->getImage(m_direction->frames.at(f).imageIndex));
	}
	dirLayout->addWidget(m_framesContainer);

	m_removeFrameButton = new QToolButton(this);
	m_removeFrameButton->setText("-");
	connect(m_removeFrameButton, SIGNAL(clicked(bool)), this, SLOT(removeFrameSlot()));
	dirLayout->addWidget(m_removeFrameButton);

	m_addFrameButton = new QToolButton(this);
	m_addFrameButton->setText("+");
	connect(m_addFrameButton, SIGNAL(clicked(bool)), this, SLOT(addFrameSlot()));
	dirLayout->addWidget(m_addFrameButton);
	dirLayout->addStretch(5);
}

SSDirectionView::~SSDirectionView() {
	disconnect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
				this, SLOT(showContextMenu(const QPoint&)));
	disconnect(m_removeFrameButton, SIGNAL(triggered(QAction*)),
				this, SLOT(removeFrameSlot()));
	disconnect(m_addFrameButton, SIGNAL(triggered(QAction*)),
				this, SLOT(addFrameSlot()));
}

void SSDirectionView::setImage(int index, QImage image) {
	m_frameViews.at(index)->setFixedSize(image.width()*m_zoomFactor+m_zoomFactor, image.height()*m_zoomFactor+m_zoomFactor);
	QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));

	if(item == nullptr) {
		QImage* first = m_spriteset->getImage(0);
		item = new QGraphicsPixmapItem(QPixmap::fromImage(*first));
	}
	item->setScale(m_zoomFactor);
	m_frameViews.at(index)->scene()->addItem(item);
}

QString SSDirectionView::getName() {
	return m_nameLineEdit->text();
}

void SSDirectionView::setName(QString name) {
	m_nameLineEdit->setText(name);
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

	QAction* result = contextMenu.exec(mapToGlobal(pos));
	if(!result) return;
	QString text = result->text();
	if(text == "Insert") {

	} else if(text == "Append") {

	}
}

void SSDirectionView::removeFrame(int index) {
	delete m_frameViews.takeAt(index);
}

int SSDirectionView::getZoom() {
	return m_zoomFactor;
}

void SSDirectionView::setZoom(int factor) {
	m_zoomFactor = factor;
}

void SSDirectionView::addFrame(QImage* image) {
	QGraphicsView* newView = new QGraphicsView(new QGraphicsScene());
	newView->setObjectName("frame" + QString::number(m_frameViews.length()));
	newView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	newView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	int scaledWidth = 32*m_zoomFactor;
	int scaledHeight = 32*m_zoomFactor;
	newView->setBackgroundBrush(QBrush(QPixmap(":/icons/transparency-bg.png")));

	QGraphicsPixmapItem* frameImage;
	if(image == nullptr) {
		frameImage = new QGraphicsPixmapItem(QPixmap::fromImage(*m_spriteset->getImage(0)));
		scaledWidth = frameImage->pixmap().width()*m_zoomFactor;
		scaledHeight = frameImage->pixmap().height()*m_zoomFactor;
	} else {
		frameImage = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
		scaledWidth = image->width()*m_zoomFactor;
		scaledHeight = image->height()*m_zoomFactor;
	}

	newView->scene()->addItem(frameImage);
	frameImage->setScale(m_zoomFactor);
	newView->setFixedSize(scaledWidth,scaledHeight);

	m_framesLayout->addWidget(newView);
	m_frameViews.append(newView);
}

void SSDirectionView::removeFrameSlot() {
	if(m_frameViews.length() > 1)
		removeFrame(m_frameViews.length()-1);
}

void SSDirectionView::addFrameSlot() {
	addFrame(m_spriteset->getImage(0));
}

void SSDirectionView::changeDirectionName(QString name) {
	m_spriteset->setDirectionName(m_directionIndex, name);
}

void SSDirectionView::mousePressEvent(QMouseEvent* event) {
	if(event->button() == Qt::LeftButton) {
		// QWidget* child = childAt(event->pos());
	}
	event->accept();
}
