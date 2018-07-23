#include <QColorDialog>
#include <QDebug>
#include <QPushButton>
#include <QPalette>
#include <QSize>
#include <QSpinBox>

#include "importoptionsdialog.h"
#include "ui_importoptionsdialog.h"

ImportOptionsDialog::ImportOptionsDialog(QWidget *parent): QDialog(parent), ui(new Ui::ImportOptionsDialog) {
	ui->setupUi(this);
	this->setButtonBackground(ui->colorInButton, Qt::magenta);
	this->setButtonBackground(ui->colorOutButton, QColor(255,0,255,0));
}

ImportOptionsDialog::~ImportOptionsDialog() {
	delete ui;
}

QSize ImportOptionsDialog::getFrameSize() {
	return this->frameSize;
}

QColor ImportOptionsDialog::getTransparencyIn() {
	return this->transparencyIn;
}

QColor ImportOptionsDialog::getTransparencyOut() {
	return this->transparencyOut;
}

bool ImportOptionsDialog::removeDuplicatesChecked() {
	return this->removeDuplicates;
}


void ImportOptionsDialog::on_colorOutButton_clicked() {
	this->setButtonBackground(ui->colorOutButton, "Color in");
}

void ImportOptionsDialog::on_colorInButton_clicked() {
	this->setButtonBackground(ui->colorInButton, "Color in");
}

void ImportOptionsDialog::setButtonBackground(QPushButton *button, char *title) {
	QColorDialog* qcd = new QColorDialog(this);
	QColor noChange = QColor(button->palette().color(QPalette::Button));
	QColor newColor = qcd->getColor(noChange, this, title, QColorDialog::ShowAlphaChannel|QColorDialog::DontUseNativeDialog);
	if(newColor.isValid())
		this->setButtonBackground(button, newColor);
	delete qcd;
}

void ImportOptionsDialog::setButtonBackground(QPushButton* button, QColor background) {
	QPalette pal = button->palette();
	pal.setColor(QPalette::Button, background);
	button->setAutoFillBackground(true);
	button->setPalette(pal);
	button->update();
}

void ImportOptionsDialog::on_buttonBox_accepted() {
	this->frameSize = QSize(ui->spriteWidth->value(), ui->spriteHeight->value());
	this->transparencyIn = QColor(ui->colorInButton->palette().color(QPalette::Button));
	this->transparencyOut = QColor(ui->colorOutButton->palette().color(QPalette::Button));
	this->removeDuplicates = ui->noDupsCheckBox;
	this->accept();
}

void ImportOptionsDialog::on_buttonBox_rejected() {
	this->reject();
}
