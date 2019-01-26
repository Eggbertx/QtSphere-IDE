#include <QColorDialog>
#include <QMouseEvent>
#include <QStyle>

#include "widgets/colorbutton.h"

ColorButton::ColorButton(QWidget *parent,QColor color): QToolButton(parent) {
	this->color = color;
	int metricSize = this->style()->pixelMetric(QStyle::PM_ButtonIconSize);
	this->pixmapSize = QSize(metricSize*2,metricSize);
	this->updateIcon();
}

void ColorButton::setColor(QColor color) {
	this->color = color;
	this->updateIcon();
}

QColor ColorButton::getColor() {
	return this->color;
}

void ColorButton::mouseReleaseEvent(QMouseEvent* e) {
	if(e->button() != Qt::LeftButton) return;
	QColor newCol = QColorDialog::getColor(this->color);
	this->setDown(false);
	if(this->color == newCol || !newCol.isValid()) return;
	this->color = newCol;
	emit this->colorChanged(this->color);
	this->updateIcon();
}


void ColorButton::updateIcon() {
	this->colorIcon = QPixmap(this->pixmapSize);
	this->colorIcon.fill(QColor(this->color));
	this->setIconSize(this->pixmapSize);
	this->setIcon(this->colorIcon);
}
