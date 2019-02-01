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

int LayerPropertiesDialog::getWidth() {
	return ui->widthSpinBox->value();
}

int LayerPropertiesDialog::getHeight() {
	return ui->heightSpinBox->value();
}

void LayerPropertiesDialog::setSize(int width, int height) {
	ui->widthSpinBox->setValue(width);
	ui->heightSpinBox->setValue(height);
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

void LayerPropertiesDialog::setReflectiveEnabled(bool enabled) {
	ui->reflective_chk->setChecked(enabled);
}

bool LayerPropertiesDialog::reflectiveEnabled() {
	return ui->reflective_chk->isChecked();
}

int LayerPropertiesDialog::getAutoScrollingWidth() {
	return ui->horizontalAsSlider->value();
}

int LayerPropertiesDialog::getAutoScrollingHeight() {
	return ui->verticalAsSlider->value();
}

void LayerPropertiesDialog::setAutoScrolling(int h, int v) {
	ui->horizontalAsSlider->setValue(h);
	ui->verticalAsSlider->setValue(v);
}

void LayerPropertiesDialog::on_parallaxChk_stateChanged(int arg1) {
	ui->horizontalParallaxSlider->setEnabled(arg1 == Qt::Checked);
	ui->verticalParallaxSlider->setEnabled(arg1 == Qt::Checked);
	ui->horizontalAsSlider->setEnabled(arg1 == Qt::Checked);
	ui->verticalAsSlider->setEnabled(arg1 == Qt::Checked);
}

void LayerPropertiesDialog::on_horizontalParallaxSlider_valueChanged(int value) {
	updateSliderIndicator(ui->horizontalParallaxSlider, ui->horizontalParallaxIndicator);
}

void LayerPropertiesDialog::on_verticalParallaxSlider_valueChanged(int value) {
	updateSliderIndicator(ui->verticalParallaxSlider, ui->verticalParallaxIndicator);
}

void LayerPropertiesDialog::on_horizontalAsSlider_valueChanged(int value) {
	updateSliderIndicator(ui->horizontalAsSlider, ui->horizontalAsIndicator);
}

void LayerPropertiesDialog::on_verticalAsSlider_valueChanged(int value) {
	updateSliderIndicator(ui->verticalAsSlider, ui->verticalAsIndicator);
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
