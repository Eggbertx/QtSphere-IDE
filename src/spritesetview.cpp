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


	Spriteset penguin(this);
	penguin.open("penguin.rss");

	ImageChooser* ic = new ImageChooser(true, this);
	ui->ssImages->setWidget(ic);
	ui->ssImages->setAlignment(Qt::AlignLeft|Qt::AlignTop);

	QList<int> sizes;
	sizes << 600 << 100;
	ui->ssViewSplitter->setSizes(sizes);
	sizes.clear();

	sizes << 200 << 150 << 200;
	ui->choosers->setSizes(sizes);
	sizes.clear();

	ic->setImages(penguin.images);
	const QPixmap animPixmap = QPixmap::fromImage(penguin.images.at(0));

	QGraphicsPixmapItem* animFrame = new QGraphicsPixmapItem(animPixmap);
	ui->animView->setScene(new QGraphicsScene(ui->animView));
	animFrame->setScale(2);
	ui->animView->scene()->addItem(animFrame);

	ui->animView->setBackgroundBrush(QBrush(QPixmap(":/icons/transparency-bg.png")));
	QList<Spriteset::rss_direction> penguin_directions = penguin.getDirections();
	int num_directions = penguin_directions.length();
	for(int d = 0; d < num_directions; d++) {
		this->addDirection("",4);
	}

	ui->dirsLayout->addStretch(5);
	//static_cast<QHBoxLayout>(ui->mainSpriteAreaLayout).addStretch(4);
}

SpritesetView::~SpritesetView() {
	delete ui;
	disconnect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(showContextMenu(const QPoint&)));
}

void SpritesetView::addDirection(QString name, int numFrames) {
	SSDirectionView* dirView = new SSDirectionView(name, numFrames);
	ui->dirsLayout->insertWidget(ui->dirsLayout->children().length(),dirView);
}

void SpritesetView::showContextMenu(const QPoint &pos) {
	qDebug() << this->childAt(pos)->objectName();

	QMenu menu(this);
	menu.addAction("Add direction");
	menu.addAction("2");
	QAction* result = menu.exec(this->mapToGlobal(pos));
	if(!result) return;
	if(result->text() == "Add direction") {
		this->addDirection("",1);
	}
}
