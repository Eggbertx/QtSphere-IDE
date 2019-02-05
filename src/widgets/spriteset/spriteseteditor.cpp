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

#include "widgets/spriteset/spriteseteditor.h"
#include "ui_spriteseteditor.h"
#include "widgets/palettewidget.h"
#include "widgets/imagechooser.h"
#include "mainwindow.h"
#include "widgets/spriteset/ssdirectionview.h"
#include "formats/spriteset.h"
#include "widgets/sphereeditor.h"

SpritesetEditor::SpritesetEditor(QWidget *parent): SphereEditor(parent), ui(new Ui::SpritesetEditor) {
	ui->setupUi(this);
	m_type = SphereEditor::SpritesetEditor;
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(showContextMenu(const QPoint&)));
	setBackgroundRole(QPalette::Shadow);
	ui->dirsLayout->setContentsMargins(8,4,4,4);
	ui->dirsLayout->setSpacing(0);
	PaletteWidget* pw = new PaletteWidget(this);

	ui->palette->setWidget(pw);
	ui->palette->setAlignment(Qt::AlignTop|Qt::AlignLeft);
	QPalette pal = palette();
	pal.setColor(QPalette::Background, QColor(40,40,40));

	m_spriteset = new Spriteset(this);
	m_images = new ImageChooser(true, this);
	ui->ssImages->setWidget(m_images);
	ui->ssImages->setAlignment(Qt::AlignLeft|Qt::AlignTop);

	int mainWidth = MainWindow::instance()->width();
	ui->ssViewSplitter->setStretchFactor(0,4);
	ui->ssViewSplitter->setSizes(QList<int>({mainWidth-164,164}));
	ui->choosers->setSizes(QList<int>({200,150,200}));

	ui->animView->setBackgroundBrush(QBrush(QPixmap(":/icons/transparency-bg.png")));
	ui->dirsLayout->addStretch(5);
}

SpritesetEditor::~SpritesetEditor() {
	delete ui;
	disconnect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
			   this, SLOT(showContextMenu(const QPoint&)));
}

bool SpritesetEditor::attach(Spriteset* spriteset) {
	m_spriteset = spriteset;
	m_images->setImages(m_spriteset->getImages());
	if(m_spriteset->numImages() == 0) return true;

	const QPixmap animPixmap = QPixmap::fromImage(*m_spriteset->getImage(0));
	QGraphicsPixmapItem* animFrame = new QGraphicsPixmapItem(animPixmap);
	ui->animView->setScene(new QGraphicsScene(ui->animView));
	animFrame->setScale(2);
	ui->animView->scene()->addItem(animFrame);
	int num_directions = m_spriteset->numDirections();
	ui->animDirChoose->clear();
	for(int d = 0; d < num_directions; d++) {
		Spriteset::SSDirection* direction = m_spriteset->getDirection(d);
		ui->animDirChoose->addItem(direction->name);
		attachDirection(d);
	}

	return true;
}

Spriteset *SpritesetEditor::getSpriteset() {
	return m_spriteset;
}

bool SpritesetEditor::openFile(QString filename) {
	bool success = m_spriteset->open(filename);
	if(!success) return false;
	return attach(m_spriteset);
}

void SpritesetEditor::addDirection(QString name, int numFrames) {
	m_spriteset->addDirection(name, numFrames);
	SSDirectionView* dirView = new SSDirectionView(m_spriteset, m_spriteset->numDirections()-1, this);
	ui->dirsContainer->addWidget(dirView);
}

void SpritesetEditor::attachDirection(int index) {
	SSDirectionView* dirView = new SSDirectionView(m_spriteset, index, this);
	ui->dirsContainer->addWidget(dirView);
}

void SpritesetEditor::showContextMenu(const QPoint &pos) {
	QMenu menu(this);
	menu.addAction("Add direction");
	QAction* result = menu.exec(mapToGlobal(pos));
	if(!result) return;
	if(result->text() == "Add direction") {
		addDirection("",1);
	}
}

void SpritesetEditor::on_animDirChoose_currentIndexChanged(int index) {

}
