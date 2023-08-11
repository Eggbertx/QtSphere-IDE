#include "layerpropertiesdialog.h"
#include "ui_layerpropertiesdialog.h"

LayerPropertiesDialog::LayerPropertiesDialog(QWidget *parent): QDialog(parent), ui(new Ui::LayerPropertiesDialog) {
	ui->setupUi(this);
	connect(ui->parallaxChk, &QCheckBox::stateChanged, this, &LayerPropertiesDialog::onParallaxChkStateChanged);
	connect(ui->horizontalParallaxSlider, &QSlider::valueChanged, this, &LayerPropertiesDialog::onHorizontalParallaxSliderValueChanged);
	connect(ui->verticalParallaxSlider, &QSlider::valueChanged, this, &LayerPropertiesDialog::onVerticalParallaxSliderValueChanged);
	connect(ui->horizontalAsSlider, &QSlider::valueChanged, this, &LayerPropertiesDialog::onHorizontalAsSliderValueChanged);
	connect(ui->verticalAsSlider, &QSlider::valueChanged, this, &LayerPropertiesDialog::onVerticalAsSliderValueChanged);
}

LayerPropertiesDialog::~LayerPropertiesDialog() {
	disconnect(ui->parallaxChk, &QCheckBox::stateChanged, this, &LayerPropertiesDialog::onParallaxChkStateChanged);
	disconnect(ui->horizontalParallaxSlider, &QSlider::valueChanged, this, &LayerPropertiesDialog::onHorizontalParallaxSliderValueChanged);
	disconnect(ui->verticalParallaxSlider, &QSlider::valueChanged, this, &LayerPropertiesDialog::onVerticalParallaxSliderValueChanged);
	disconnect(ui->horizontalAsSlider, &QSlider::valueChanged, this, &LayerPropertiesDialog::onHorizontalAsSliderValueChanged);
	disconnect(ui->verticalAsSlider, &QSlider::valueChanged, this, &LayerPropertiesDialog::onVerticalAsSliderValueChanged);
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

void LayerPropertiesDialog::onParallaxChkStateChanged(int arg1) {
	ui->horizontalParallaxSlider->setEnabled(arg1 == Qt::Checked);
	ui->verticalParallaxSlider->setEnabled(arg1 == Qt::Checked);
	ui->horizontalAsSlider->setEnabled(arg1 == Qt::Checked);
	ui->verticalAsSlider->setEnabled(arg1 == Qt::Checked);
}

void LayerPropertiesDialog::onHorizontalParallaxSliderValueChanged(int value) {
	updateSliderIndicator(ui->horizontalParallaxSlider, ui->horizontalParallaxIndicator);
}

void LayerPropertiesDialog::onVerticalParallaxSliderValueChanged(int value) {
	updateSliderIndicator(ui->verticalParallaxSlider, ui->verticalParallaxIndicator);
}

void LayerPropertiesDialog::onHorizontalAsSliderValueChanged(int value) {
	updateSliderIndicator(ui->horizontalAsSlider, ui->horizontalAsIndicator);
}

void LayerPropertiesDialog::onVerticalAsSliderValueChanged(int value) {
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
