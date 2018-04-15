#include <QDebug>
#include <QColor>
#include <QMenu>
#include <QAction>
#include <QPalette>
#include <QPen>
#include <QPainter>
#include <QPaintEvent>
#include "palettewidget.h"

PaletteWidget::PaletteWidget(QWidget *parent): QWidget(parent) {
	this->installEventFilter(this);
	this->paletteColors = QList<QColor>();
	QPalette pal = palette();
	pal.setColor(QPalette::Background, Qt::black);
	this->setAutoFillBackground(true);
	this->setPalette(pal);

	this->rightClickMenu = new QMenu(this);
	QMenu* fileMenu = this->rightClickMenu->addMenu("File");
	fileMenu->addAction("New");
	fileMenu->addAction("Load...");
	fileMenu->addAction("Save...");

	QMenu* defaultMenu = this->rightClickMenu->addMenu("Default");
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
}

PaletteWidget::~PaletteWidget() {

}

void PaletteWidget::changePalette(QColor palettearr[], int numColors) {
	this->paletteColors.clear();
	for(int c = 0; c < numColors; c++) {
		this->paletteColors << palettearr[c];
	}
}

bool PaletteWidget::eventFilter(QObject* object, QEvent* event) {
	event->accept();
	switch(event->type()) {
		case QEvent::MouseMove: {
			QMouseEvent* e = static_cast<QMouseEvent*>(event);
			this->mousePos = e->pos();
			break;
		}
		case QEvent::Paint: {
			QPainter painter(this);
			int x = 0;
			int y = 0;

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
			if(buttonPressed == Qt::LeftButton) {
				int numColors = this->paletteColors.length();
				int x = 0;
				int y = 0;
				for(int c = 0; c < numColors; c++) {
					if(x <= me->pos().x() && me->pos().x() < x + this->squareSize &&
						y <= me->pos().y() && me->pos().y() < y + this->squareSize) {
						this->selectedIndex = c;
						this->repaint();
					}

					if(x + this->squareSize > this->width()) {
						x = 0;
						y += this->squareSize;
					}
					x+= this->squareSize;
				}
			} else if(buttonPressed == Qt::RightButton){
				QAction* choice = this->rightClickMenu->exec(this->mapToGlobal(pos));
				if(!choice) return QObject::eventFilter(object, event);
				if(choice->text() == "New") {
					this->paletteColors.clear();
					this->selectedIndex = 0;
					repaint();
				} else if(choice->text() == "DOS") {
					this->changePalette(dos_palette, 256);
					this->repaint();
				} else if(choice->text() == "VERGE") {
					this->changePalette(verge_palette, 256);
					this->repaint();
				} else if(choice->text() == "PLASMA") {
					this->changePalette(plasma_palette, 256);
					this->repaint();
				} else if(choice->text() == "RGB332") {
					this->changePalette(rgb332_palette, 256);
					this->repaint();
				} else if(choice->text() == "VISIBONE2") {
					this->changePalette(visibone2_palette, 256);
					this->repaint();
				} else if(choice->text() == "HSL256") {
					this->changePalette(hsl256_palette, 256);
					this->repaint();
				}
				QObject::eventFilter(object, event);
			}
			break;
		}
		default:
			break;
	}
	return true;
}
