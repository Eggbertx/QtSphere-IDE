#include "drawingwidget.h"
#include "ui_drawingwidget.h"

DrawingWidget::DrawingWidget(QWidget *parent): QWidget(parent), ui(new Ui::DrawingWidget) {
	ui->setupUi(this);
}

DrawingWidget::~DrawingWidget() {
	delete ui;
}
