#include <QWidget>
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
#include "widgets/spriteset/spriteseteditor.h"
#include "palette_presets.h"

PaletteWidget::PaletteWidget(SpritesetEditor *parent): QWidget(parent) {
	installEventFilter(this);
	m_paletteColors = QList<QColor>();
	changePalette(hsl256_palette);

	m_rightClickMenu = new QMenu(this);
	m_fileMenu = m_rightClickMenu->addMenu("File");
	m_fileMenu->addAction("New");
	m_fileMenu->addAction("Import...");
	m_fileMenu->addAction("Export...");

	m_defaultMenu = m_rightClickMenu->addMenu("Presets");
	m_defaultMenu->addAction("Commodore 64");
	m_defaultMenu->addAction("DOS");
	m_defaultMenu->addAction("HSL256");
	m_defaultMenu->addAction("NES");
	m_defaultMenu->addAction("PLASMA");
	m_defaultMenu->addAction("RGB332");
	m_defaultMenu->addAction("VERGE");
	m_defaultMenu->addAction("VISIBONE2");

	m_rightClickMenu->addSeparator();
	m_rightClickMenu->addAction("Insert before");
	m_rightClickMenu->addAction("Insert after");
	m_rightClickMenu->addAction("Replace");
	m_rightClickMenu->addAction("Remove");
	m_selectedIndex = 0;
	parent->setMinimumSize(160,160);
	repaint();
}

PaletteWidget::~PaletteWidget() {
	delete m_fileMenu;
	delete m_defaultMenu;
	delete m_rightClickMenu;
}

void PaletteWidget::changePalette(QList<QColor> palette) {
	m_paletteColors.clear();
	foreach(QColor color, palette) {
		m_paletteColors << color;
	}
	repaint();
}

void PaletteWidget::importPalette(const QString path) {
	QFile* pal = new QFile(path);
	if(!pal->open(QIODevice::ReadOnly)) return;
	m_paletteColors.clear();
	char lineText[8];
	QColor newCol;
	while(pal->readLine(lineText, sizeof(lineText)) != -1) {
		newCol = QColor(QString(lineText).replace("\n",""));
		if(newCol.isValid()) m_paletteColors.append(QColor(lineText));
	}
	pal->close();
	delete pal;
	repaint();
}

void PaletteWidget::exportPalette(QString path) {
	if(QFileInfo(path).suffix() == "") path += ".qsipal";
	QFile* pal = new QFile(path);
	if(!pal->open((QIODevice::WriteOnly|QIODevice::Text))) return;
	for(int i = 0; i < m_paletteColors.length(); i++ ) {
		pal->write(m_paletteColors.at(i).name().toStdString().append("\n").c_str());
	}
	pal->close();
	delete pal;
}

bool PaletteWidget::eventFilter(QObject* object, QEvent* event) {
	(void)object;
	switch(event->type()) {
		case QEvent::MouseMove: {
			m_mousePos = dynamic_cast<QMouseEvent*>(event)->pos();
			break;
		}
		case QEvent::Paint: {
			QPainter painter(this);
			int x = 0;
			int y = 0;
			for(int c=0; c < m_paletteColors.length(); c++) {
				if(x + m_squareSize > width()) {
					x = 0;
					y += m_squareSize;
				}
				if(m_selectedIndex == c) {
					painter.fillRect(QRect(x,y,m_squareSize,m_squareSize),Qt::black);
					painter.fillRect(QRect(x+1,y+1,m_squareSize-2,m_squareSize-2),Qt::white);
					painter.fillRect(QRect(x+2,y+2,m_squareSize-4,m_squareSize-4),Qt::black);
					painter.fillRect(QRect(x+3,y+3,m_squareSize-6,m_squareSize-6),m_paletteColors.at(c));
				} else {
					painter.fillRect(QRect(x,y,m_squareSize,m_squareSize),m_paletteColors.at(c));
				}
				x += m_squareSize;
			}
			break;
		}
		case QEvent::MouseButtonPress: {
			QMouseEvent* me = dynamic_cast<QMouseEvent*>(event);
			int buttonPressed = me->button();
			QPoint pos = me->pos();

			if(buttonPressed == (Qt::LeftButton|Qt::RightButton)) {
				int numColors = m_paletteColors.length();
				int x = 0;
				int y = 0;
				for(int c = 0; c < numColors; c++) {
					if(x + m_squareSize > width()) {
						x = 0;
						y += m_squareSize;
					}
					if(x <= pos.x() && pos.x() <= x + m_squareSize
					&& y <= pos.y() && pos.y() <= y + m_squareSize) {
						m_selectedIndex = c;
						repaint();
					}
					x+= m_squareSize;
				}
			}
			if(buttonPressed == Qt::RightButton){
				QAction* choice = m_rightClickMenu->exec(mapToGlobal(pos));
				if(!choice) return true;
				QString text = choice->text();
				if(text == "New") {
					m_selectedIndex = 0;
					m_paletteColors.clear();
					m_paletteColors << QColor();
					repaint();
				} else if(text == "Import...") {
					QString importPath = QFileDialog::getOpenFileName(this, "Import Palette", QDir().absolutePath(),
						"QtSphere IDE palettes (*.qsipal);;All files (*.*)");
					if(importPath != "") importPalette(importPath);
				} else if(text == "Export...") {
					QString exportPath = QFileDialog::getSaveFileName(this, "Export Palette", QDir().absolutePath(),
						"QtSphere IDE palettes (*.qsipal);;All files (*.*)");
					if(exportPath != "") exportPalette(exportPath);
				} else if(text == "Commodore 64") {
					changePalette(c64_palette);
				} else if(text == "DOS") {
					changePalette(dos_palette);
				} else if(text == "NES") {
					changePalette(nes_palette);
				} else if(text == "VERGE") {
					changePalette(verge_palette);
				} else if(text == "PLASMA") {
					changePalette(plasma_palette);
				} else if(text == "RGB332") {
					changePalette(rgb332_palette);
				} else if(text == "VISIBONE2") {
					changePalette(visibone2_palette);
				} else if(text == "HSL256") {
					changePalette(hsl256_palette);
				} else if(text == "Insert before") {
					QColor newCol = QColorDialog::getColor(Qt::white,this);
					if(newCol.isValid()) {
						m_paletteColors.insert(m_selectedIndex++, newCol);
						repaint();
					}
				} else if(text == "Insert after") {
					QColor newCol = QColorDialog::getColor(Qt::white, this);
					if(newCol.isValid()) {
						m_paletteColors.insert(m_selectedIndex+1, newCol);
						repaint();
					}
				} else if(text == "Replace") {
					QColor newCol = QColorDialog::getColor(Qt::white, this);
					if(newCol.isValid()) {
						m_paletteColors.replace(m_selectedIndex, newCol);
						repaint();
					}
				} else if(text == "Remove") {
					m_paletteColors.removeAt(m_selectedIndex);
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
