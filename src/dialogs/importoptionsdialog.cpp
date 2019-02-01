#include <QColorDialog>
#include <QDebug>
#include <QPushButton>
#include <QPalette>
#include <QSize>
#include <QSpinBox>

#include "dialogs/importoptionsdialog.h"
#include "ui_importoptionsdialog.h"

ImportOptionsDialog::ImportOptionsDialog(QWidget *parent): QDialog(parent), ui(new Ui::ImportOptionsDialog) {
	ui->setupUi(this);
	setButtonBackground(ui->colorInButton, Qt::magenta);
	setButtonBackground(ui->colorOutButton, QColor(255,0,255,0));
}

ImportOptionsDialog::~ImportOptionsDialog() {
	delete ui;
}

QSize ImportOptionsDialog::getFrameSize() {
	return m_frameSize;
}

QColor ImportOptionsDialog::getTransparencyIn() {
	return m_transparencyIn;
}

QColor ImportOptionsDialog::getTransparencyOut() {
	return m_transparencyOut;
}

bool ImportOptionsDialog::removeDuplicatesChecked() {
	return m_removeDuplicates;
}


void ImportOptionsDialog::on_colorOutButton_clicked() {
	setButtonBackground(ui->colorOutButton, "Color in");
}

void ImportOptionsDialog::on_colorInButton_clicked() {
	setButtonBackground(ui->colorInButton, "Color in");
}

void ImportOptionsDialog::setButtonBackground(QPushButton *button, char *title) {
	QColorDialog* qcd = new QColorDialog(this);
	QColor noChange = QColor(button->palette().color(QPalette::Button));
	QColor newColor = qcd->getColor(noChange, this, title, QColorDialog::ShowAlphaChannel|QColorDialog::DontUseNativeDialog);
	if(newColor.isValid())
		setButtonBackground(button, newColor);
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
	m_frameSize = QSize(ui->spriteWidth->value(), ui->spriteHeight->value());
	m_transparencyIn = QColor(ui->colorInButton->palette().color(QPalette::Button));
	m_transparencyOut = QColor(ui->colorOutButton->palette().color(QPalette::Button));
	m_removeDuplicates = ui->noDupsCheckBox;
	accept();
}

void ImportOptionsDialog::on_buttonBox_rejected() {
	reject();
}
