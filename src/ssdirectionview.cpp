#include <QGraphicsView>
#include <QLayout>
#include <QLineEdit>
#include <QMenu>
#include <QPalette>
#include <QPushButton>
#include <QStyle>
#include <QToolButton>

#include "ssdirectionview.h"
#include "util.h"

SSDirectionView::SSDirectionView(QString name, int numFrames, QWidget *parent): QFrame(parent) {
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	//this->setFrameStyle(QFrame::Panel | QFrame::Raised);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(showContextMenu(const QPoint&)));
	/*QPalette pal = palette();
	pal.setColor(QPalette::Background, QColor(40,40,40));
	this->setAutoFillBackground(true);
	this->setPalette(pal);*/

	QHBoxLayout* dirLayout = new QHBoxLayout();
	dirLayout->setSpacing(0);
	dirLayout->setMargin(0);
	this->setLayout(dirLayout);

	this->nameLineEdit = new QLineEdit(name);
	this->nameLineEdit->setFixedWidth(100);
	dirLayout->addWidget(this->nameLineEdit);
	this->framesContainer = new QWidget();
	//this->framesLayout = new QGridLayout();
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

void SSDirectionView::addFrame(QImage *image) {
	QGraphicsView* newView = new QGraphicsView(new QGraphicsScene());
	newView->setFixedSize(60,60);
	newView->setBackgroundBrush(QBrush(QPixmap(":/icons/transparency-bg.png")));
	//this->framesContainer->layout()->addWidget(newView);
	this->framesLayout->addWidget(newView);
	this->frameViews.append(newView);
}

void SSDirectionView::removeFrameSlot() {
	if(this->frameViews.length() > 1)
		this->removeFrame(this->frameViews.length()-1);
}

void SSDirectionView::addFrameSlot() {
	this->addFrame(nullptr);
}
