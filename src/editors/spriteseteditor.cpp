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

#include "spriteseteditor.h"
#include "ui_spriteseteditor.h"
#include "palettewidget.h"
#include "imagechooser.h"
#include "editors/ssdirectionview.h"
#include "formats/spriteset.h"
#include "editors/sphereeditor.h"

SpritesetEditor::SpritesetEditor(QWidget *parent): SphereEditor (parent), ui(new Ui::SpritesetEditor) {
	ui->setupUi(this);
	this->type = SphereEditor::SpritesetEditor;
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

SpritesetEditor::~SpritesetEditor() {
	delete ui;
	disconnect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			   this, SLOT(showContextMenu(const QPoint&)));
}

bool SpritesetEditor::attach(Spriteset* spriteset) {
	this->spriteset = spriteset;
	this->images->setImages(spriteset->images);
	if(this->spriteset->images.length() == 0) return true;

	const QPixmap animPixmap = QPixmap::fromImage(this->spriteset->images.at(0));
	QGraphicsPixmapItem* animFrame = new QGraphicsPixmapItem(animPixmap);
	ui->animView->setScene(new QGraphicsScene(ui->animView));
	animFrame->setScale(2);
	ui->animView->scene()->addItem(animFrame);

	int num_directions = this->spriteset->directions.length();
	ui->animDirChoose->clear();
	for(int d = 0; d < num_directions; d++) {
		Spriteset::SSDirection direction = this->spriteset->directions.at(d);
		ui->animDirChoose->addItem(direction.name);
		this->addDirection(direction);
	}

	return true;
}

bool SpritesetEditor::openFile(QString filename) {
	bool success = this->spriteset->open(filename);
	if(!success) return false;
	return this->attach(this->spriteset);
}

void SpritesetEditor::addDirection(QString name, int numFrames) {
	Spriteset::SSDirection direction;
	direction.name = "";
	direction.frames = QList<Spriteset::SSFrame>();
	Spriteset::SSFrame frame;
	frame.delay = 8;
	frame.imageIndex = 1;
	direction.frames.append(frame);

	SSDirectionView* dirView = new SSDirectionView(name, numFrames, &direction, this);
	ui->dirsContainer->addWidget(dirView);
}

void SpritesetEditor::addDirection(Spriteset::SSDirection direction) {
	SSDirectionView* dirView = new SSDirectionView(direction.name, direction.frames.length(), &direction, this);

	for(int f = 0; f < direction.frames.length(); f++) {
		QImage frameImage = static_cast<QImage>(this->spriteset->images.at(direction.frames.at(f).imageIndex));
		dirView->setImage(f, frameImage);
	}
	ui->dirsContainer->addWidget(dirView);
}

void SpritesetEditor::showContextMenu(const QPoint &pos) {
	QMenu menu(this);
	menu.addAction("Add direction");
	QAction* result = menu.exec(this->mapToGlobal(pos));
	if(!result) return;
	if(result->text() == "Add direction") {
		this->addDirection("",1);
	}
}

void SpritesetEditor::on_animDirChoose_currentIndexChanged(int index) {

}
