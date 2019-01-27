#include "layerpropertiesdialog.h"
#include "ui_layerpropertiesdialog.h"

LayerPropertiesDialog::LayerPropertiesDialog(QWidget *parent): QDialog(parent), ui(new Ui::LayerPropertiesDialog) {
	ui->setupUi(this);
}

LayerPropertiesDialog::~LayerPropertiesDialog() {
	delete ui;
}

QString LayerPropertiesDialog::getName() {
	return ui->nameLineEdit->text();
}

void LayerPropertiesDialog::setName(QString name) {
	ui->nameLineEdit->setText(name);
}

int LayerPropertiesDialog::getHorizontalParallax() {
	return ui->horizontalParallaxSlider->value();
}

int LayerPropertiesDialog::getVerticalParallax() {
	return ui->verticalParallaxSlider->value();
}

void LayerPropertiesDialog::setParallax(int h, int v) {
	ui->horizontalParallaxSlider->setValue(h);
	ui->verticalParallaxSlider->setValue(v);
}

bool LayerPropertiesDialog::parallaxEnabled() {
	return ui->parallaxChk->isChecked();
}

void LayerPropertiesDialog::setParallaxEnabled(bool enabled) {
	ui->parallaxChk->setChecked(enabled);
}

bool LayerPropertiesDialog::reflectiveEnabled() {
	return ui->reflective_chk->isChecked();
}


void LayerPropertiesDialog::on_reflective_chk_stateChanged(int arg1) {
	ui->horizontalAsSlider->setEnabled(arg1 == Qt::Checked);
	ui->verticalAsSlider->setEnabled(arg1 == Qt::Checked);
}

void LayerPropertiesDialog::on_parallaxChk_stateChanged(int arg1) {
	ui->horizontalParallaxSlider->setEnabled(arg1 == Qt::Checked);
	ui->verticalParallaxSlider->setEnabled(arg1 == Qt::Checked);
}

void LayerPropertiesDialog::on_horizontalParallaxSlider_valueChanged(int value) {
	this->updateSliderIndicator(ui->horizontalParallaxSlider, ui->horizontalParallaxIndicator);
}

void LayerPropertiesDialog::on_verticalParallaxSlider_valueChanged(int value) {
	this->updateSliderIndicator(ui->verticalParallaxSlider, ui->verticalParallaxIndicator);
}

void LayerPropertiesDialog::on_horizontalAsSlider_valueChanged(int value) {
	this->updateSliderIndicator(ui->horizontalAsSlider, ui->horizontalAsIndicator);
}

void LayerPropertiesDialog::on_verticalAsSlider_valueChanged(int value) {
	this->updateSliderIndicator(ui->verticalAsSlider, ui->verticalAsIndicator);
}

void LayerPropertiesDialog::updateSliderIndicator(QSlider* slider, QLabel* indicator) {
	QString padding = "";
	int val = slider->value();
	QString valStr= QString::number(val);

	do {
		padding += " ";
	} while(padding.length() + valStr.length() < 4);
	indicator->setText(padding + valStr);
}
