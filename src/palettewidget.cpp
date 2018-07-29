#include <QDebug>
#include <QColor>
#include <QColorDialog>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QPalette>
#include <QPainter>
#include <QSizePolicy>
#include <QPaintEvent>
#include "palettewidget.h"
#include "spritesetview.h"

PaletteWidget::PaletteWidget(SpritesetView *parent): QWidget(parent) {
	this->installEventFilter(this);
	this->paletteColors = QList<QColor>();
	//QPalette pal = palette();
	//pal.setColor(QPalette::Background, Qt::black);
	//this->setAutoFillBackground(true);
	//this->setPalette(pal);

	this->rightClickMenu = new QMenu(this);
	QMenu* fileMenu = this->rightClickMenu->addMenu("File");
	fileMenu->addAction("New");
	fileMenu->addAction("Import...");
	fileMenu->addAction("Export...");

	QMenu* defaultMenu = this->rightClickMenu->addMenu("Presets");
	defaultMenu->addAction("DOS");
	defaultMenu->addAction("VERGE");
	defaultMenu->addAction("PLASMA");
	defaultMenu->addAction("RGB332");
	defaultMenu->addAction("VISIBONE2");
	defaultMenu->addAction("HSL256");

	this->rightClickMenu->addSeparator();
	this->rightClickMenu->addAction("Insert before");
	this->rightClickMenu->addAction("Insert after");
	this->rightClickMenu->addAction("Replace");
	this->rightClickMenu->addAction("Remove");
	this->selectedIndex = 0;
	this->paletteColors << QColor();
	parent->setMinimumSize(160,160);
	this->repaint();
}

PaletteWidget::~PaletteWidget() {
	delete this->rightClickMenu;
}

void PaletteWidget::changePalette(QColor palettearr[], int numColors) {
	this->paletteColors.clear();
	for(int c = 0; c < numColors; c++) {
		this->paletteColors << palettearr[c];
	}
	repaint();
}

void PaletteWidget::importPalette(QString path) {
	QFile* pal = new QFile(path);
	if(!pal->open(QIODevice::ReadOnly)) return;
	this->paletteColors.clear();
	char lineText[8];
	QColor newCol;
	while(pal->readLine(lineText, sizeof(lineText)) != -1) {
		newCol = QColor(QString(lineText).replace("\n",""));
		if(newCol.isValid()) this->paletteColors.append(QColor(lineText));
	}
	pal->close();
	delete pal;
	repaint();
}

void PaletteWidget::exportPalette(QString path) {
	if(QFileInfo(path).suffix() == "") path += ".qsipal";
	QFile* pal = new QFile(path);
	if(!pal->open((QIODevice::WriteOnly|QIODevice::Text))) return;
	for(int i = 0; i < this->paletteColors.length(); i++ ) {
		pal->write(this->paletteColors.at(i).name().toStdString().append("\n").c_str());
	}
	pal->close();
	delete pal;
}

bool PaletteWidget::eventFilter(QObject* object, QEvent* event) {
	(void)object;
	switch(event->type()) {
		case QEvent::MouseMove: {
			this->mousePos = static_cast<QMouseEvent*>(event)->pos();
			break;
		}
		case QEvent::Paint: {
			QPainter painter(this);
			int x = 0;
			int y = 0;
			//painter.fillRect(this->rect(),Qt::black);
			for(int c=0; c < this->paletteColors.length(); c++) {
				if(x + this->squareSize > this->width()) {
					x = 0;
					y += this->squareSize;
				}
				if(this->selectedIndex == c) {
					painter.fillRect(QRect(x,y,this->squareSize,this->squareSize),Qt::black);
					painter.fillRect(QRect(x+1,y+1,this->squareSize-2,this->squareSize-2),Qt::white);
					painter.fillRect(QRect(x+2,y+2,this->squareSize-4,this->squareSize-4),Qt::black);
					painter.fillRect(QRect(x+3,y+3,this->squareSize-6,this->squareSize-6),this->paletteColors.at(c));
				} else {
					painter.fillRect(QRect(x,y,this->squareSize,this->squareSize),this->paletteColors.at(c));
				}
				x+=this->squareSize;
			}
			break;
		}
		case QEvent::MouseButtonPress: {
			QMouseEvent* me = static_cast<QMouseEvent*>(event);
			int buttonPressed = me->button();
			QPoint pos = me->pos();

			if(buttonPressed == Qt::LeftButton|Qt::RightButton) {
				int numColors = this->paletteColors.length();
				int x = 0;
				int y = 0;
				for(int c = 0; c < numColors; c++) {
					if(x + this->squareSize > this->width()) {
						x = 0;
						y += this->squareSize;
					}
					if(x <= pos.x() && pos.x() <= x + this->squareSize
					&& y <= pos.y() && pos.y() <= y + this->squareSize) {
						this->selectedIndex = c;
						this->repaint();
					}
					x+= this->squareSize;
				}
			}
			if(buttonPressed == Qt::RightButton){
				QAction* choice = this->rightClickMenu->exec(this->mapToGlobal(pos));
				if(!choice) return true;
				QString text = choice->text();
				if(text == "New") {
					this->selectedIndex = 0;
					this->paletteColors.clear();
					this->paletteColors << QColor();
					repaint();
				} else if(text == "Import...") {
					QString importPath = QFileDialog::getOpenFileName(this, "Import Palette", QDir().absolutePath(),
						"QtSphere IDE palettes (*.qsipal);;All files (*.*)");
					if(importPath != "") importPalette(importPath);
				} else if(text == "Export...") {
					QString exportPath = QFileDialog::getSaveFileName(this, "Export Palette", QDir().absolutePath(),
						"QtSphere IDE palettes (*.qsipal);;All files (*.*)");
					if(exportPath != "") exportPalette(exportPath);
				} else if(text == "DOS") {
					this->changePalette(dos_palette, 256);
				} else if(text == "VERGE") {
					this->changePalette(verge_palette, 256);
				} else if(text == "PLASMA") {
					this->changePalette(plasma_palette, 256);
				} else if(text == "RGB332") {
					this->changePalette(rgb332_palette, 256);
				} else if(text == "VISIBONE2") {
					this->changePalette(visibone2_palette, 256);
				} else if(text == "HSL256") {
					this->changePalette(hsl256_palette, 256);
				} else if(text == "Insert before") {
					QColor newCol = QColorDialog::getColor(Qt::white,this);
					if(newCol.isValid()) {
						this->paletteColors.insert(this->selectedIndex++, newCol);
						repaint();
					}
				} else if(text == "Insert after") {
					QColor newCol = QColorDialog::getColor(Qt::white, this);
					if(newCol.isValid()) {
						this->paletteColors.insert(this->selectedIndex+1, newCol);
						repaint();
					}
				} else if(text == "Replace") {
					QColor newCol = QColorDialog::getColor(Qt::white, this);
					if(newCol.isValid()) {
						this->paletteColors.replace(this->selectedIndex, newCol);
						repaint();
					}
				} else if(text == "Remove") {
					this->paletteColors.removeAt(this->selectedIndex);
					repaint();
				}
			}
			break;
		}
		default:
			break;
	}

	return true;
	event->accept();
}
