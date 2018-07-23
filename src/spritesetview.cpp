#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLineEdit>
#include <QToolButton>
#include <QFileInfo>

#include "spritesetview.h"
#include "ui_spritesetview.h"
#include "palettewidget.h"
#include "imagechooser.h"
#include "ssdirectionview.h"
#include "objects/spriteset.h"

SpritesetView::SpritesetView(QWidget *parent): QWidget(parent), ui(new Ui::SpritesetView) {
	ui->setupUi(this);

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(showContextMenu(const QPoint&)));
	this->setBackgroundRole(QPalette::Shadow);
	ui->dirsLayout->setContentsMargins(8,4,4,4);
	ui->dirsLayout->setSpacing(0);
	PaletteWidget* pw = new PaletteWidget(this);

	ui->palette->setWidget(pw);
	ui->palette->setAlignment(Qt::AlignTop|Qt::AlignLeft);
	QPalette pal = palette();
	pal.setColor(QPalette::Background, QColor(40,40,40));

	this->spriteset = new Spriteset(this);
	this->images = new ImageChooser(true, this);
	ui->ssImages->setWidget(this->images);
	ui->ssImages->setAlignment(Qt::AlignLeft|Qt::AlignTop);

	QList<int> sizes;
	sizes << 600 << 100;
	ui->ssViewSplitter->setSizes(sizes);
	sizes.clear();
	sizes << 200 << 150 << 200;
	ui->choosers->setSizes(sizes);
	sizes.clear();

	ui->animView->setBackgroundBrush(QBrush(QPixmap(":/icons/transparency-bg.png")));

	ui->dirsLayout->addStretch(5);
}

SpritesetView::~SpritesetView() {
	delete ui;
	disconnect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			   this, SLOT(showContextMenu(const QPoint&)));
}

bool SpritesetView::attach(Spriteset* spriteset) {
	this->spriteset = spriteset;
	this->images->setImages(spriteset->images);
	if(this->spriteset->images.length() == 0) return true;

	const QPixmap animPixmap = QPixmap::fromImage(this->spriteset->images.at(0));
	QGraphicsPixmapItem* animFrame = new QGraphicsPixmapItem(animPixmap);
	ui->animView->setScene(new QGraphicsScene(ui->animView));
	animFrame->setScale(2);
	ui->animView->scene()->addItem(animFrame);

	int num_directions = this->spriteset->directions.length();
	for(int d = 0; d < num_directions; d++) {
		Spriteset::SSDirection direction = this->spriteset->directions.at(d);
		this->addDirection(direction);
	}

	return true;
}

bool SpritesetView::openFile(QString filename) {
	bool success = this->spriteset->open(filename);
	if(!success) return false;
	return this->attach(this->spriteset);
}

void SpritesetView::addDirection(QString name, int numFrames) {
	SSDirectionView* dirView = new SSDirectionView(name, numFrames);
	ui->dirsContainer->addWidget(dirView);
}

void SpritesetView::addDirection(Spriteset::SSDirection direction) {
	SSDirectionView* dirView = new SSDirectionView(direction.name, direction.frames.length());

	for(int f = 0; f < direction.frames.length(); f++) {
		QImage frameImage = static_cast<QImage>(this->spriteset->images.at(direction.frames.at(f).imageIndex));
		dirView->setImage(f, frameImage);
	}
	ui->dirsContainer->addWidget(dirView);
}

void SpritesetView::showContextMenu(const QPoint &pos) {
	QMenu menu(this);
	menu.addAction("Add direction");
	QAction* result = menu.exec(this->mapToGlobal(pos));
	if(!result) return;
	if(result->text() == "Add direction") {
		this->addDirection("",1);
	}
}
