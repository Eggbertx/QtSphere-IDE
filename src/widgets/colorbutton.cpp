#include <QColorDialog>
#include <QMouseEvent>
#include <QStyle>

#include "widgets/colorbutton.h"

ColorButton::ColorButton(QWidget *parent,QColor color): QToolButton(parent) {
	m_color = color;
	int metricSize = style()->pixelMetric(QStyle::PM_ButtonIconSize);
	m_pixmapSize = QSize(metricSize*2,metricSize);
	updateIcon();
}

void ColorButton::setColor(QColor color) {
	m_color = color;
	updateIcon();
}

QColor ColorButton::getColor() {
	return m_color;
}

void ColorButton::mouseReleaseEvent(QMouseEvent* e) {
	if(e->button() != Qt::LeftButton) return;
	QColor newCol = QColorDialog::getColor(m_color);
	setDown(false);
	if(m_color == newCol || !newCol.isValid()) return;
	m_color = newCol;
	emit colorChanged(m_color);
	updateIcon();
}


void ColorButton::updateIcon() {
	m_colorIcon = QPixmap(m_pixmapSize);
	m_colorIcon.fill(QColor(m_color));
	setIconSize(m_pixmapSize);
	setIcon(m_colorIcon);
}
