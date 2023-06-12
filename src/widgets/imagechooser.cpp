#include <QEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QList>
#include <QImage>
#include <QPoint>
#include <QPalette>
#include <QPixmap>
#include "widgets/imagechooser.h"
#include "formats/spriteset.h"

ImageChooser::ImageChooser(bool showTransparency, QWidget *parent): QWidget(parent) {
	installEventFilter(this);
	m_showTransparency = showTransparency;

	m_alphaBG = QPixmap::fromImage(QImage(":/icons/transparency-bg.png"));
	m_images = QList<QImage>();
	QPalette pal = palette();
	pal.setColor(QPalette::Window, Qt::black);
	setAutoFillBackground(true);
	setPalette(pal);
	m_rightClickMenu = new QMenu(this);
	QMenu* fileMenu = m_rightClickMenu->addMenu("File");
	fileMenu->addAction("Save frame");
	fileMenu->addAction("Replace frame");
	fileMenu->addAction("Append from file");
	m_rightClickMenu->addAction("Append");
	m_rightClickMenu->addAction("Remove");
	m_rightClickMenu->addAction("Remove #");
}

ImageChooser::~ImageChooser() {

}

QList<QImage> ImageChooser::getImages() {
	return m_images;
}

void ImageChooser::setImages(QList<QImage> imglist) {
	m_images = imglist;
	repaint();
}

void ImageChooser::setImages(QImage imgarr[], int num) {
	m_images.clear();
	for(int i = 0; i < num; i++) {
		m_images << imgarr[i];
	}
}

bool ImageChooser::eventFilter(QObject* object, QEvent* event) {
	switch (event->type()) {
		case QEvent::Paint: {
			QPainter painter(this);
			painter.drawTiledPixmap(rect(), m_alphaBG);
			int x = 0;
			int y = 0;
			for(int i = 0; i < m_images.length();i++) {
				if(x + m_images.at(i).width() > width()) {
					x = 0;
					y += m_images.at(i).height();
				}
				painter.drawImage(QPoint(x,y),m_images.at(i));
				if(i == m_selectedIndex) {
					painter.setPen(Qt::magenta);
					painter.drawRect(x,y,m_images.at(i).width(),m_images.at(i).height());
				}
				x += m_images.at(i).width();
			}
			break;
		}
		case QEvent::MouseMove: {
			QMouseEvent* me = dynamic_cast<QMouseEvent*>(event);
			m_mousePos = me->pos();
			break;
		}
		case QEvent::MouseButtonPress: {
			QMouseEvent* me = dynamic_cast<QMouseEvent*>(event);
			int buttonPressed = me->button();
			if(buttonPressed == Qt::LeftButton || buttonPressed == Qt::RightButton) {
				int numImages = m_images.length();
				int x = 0;
				int y = 0;
				for(int i = 0; i < numImages; i++) {
					if(x + m_images.at(i).width() > width()) {
						x = 0;
						y += m_images.at(i).height();
					}
					if(x <= me->pos().x() && me->pos().x() < x + m_images.at(i).width() &&
						y <= me->pos().y() && me->pos().y() < y + m_images.at(i).height()) {
						m_selectedIndex = i;
						repaint();
					}
					x+= m_images.at(i).width();
				}
			}
			if(buttonPressed == Qt::RightButton) {
				m_rightClickMenu->exec(mapToGlobal(me->pos()));
			}
		}
		break;
		default:
			break;
	}
	(void)object;
	return true;
}
