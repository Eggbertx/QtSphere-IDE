#include <QColorDialog>
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
	connect(ui->colorOutButton, &QPushButton::clicked, this, &ImportOptionsDialog::onColorOutButtonClicked);
	connect(ui->colorInButton, &QPushButton::clicked, this, &ImportOptionsDialog::onColorInButtonClicked);
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ImportOptionsDialog::onButtonBoxAccepted);
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ImportOptionsDialog::onButtonBoxRejected);
}

ImportOptionsDialog::~ImportOptionsDialog() {
	disconnect(ui->colorOutButton, &QPushButton::clicked, this, &ImportOptionsDialog::onColorOutButtonClicked);
	disconnect(ui->colorInButton, &QPushButton::clicked, this, &ImportOptionsDialog::onColorInButtonClicked);
	disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ImportOptionsDialog::onButtonBoxAccepted);
	disconnect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ImportOptionsDialog::onButtonBoxRejected);
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


void ImportOptionsDialog::onColorOutButtonClicked() {
	setButtonBackground(ui->colorOutButton, (char*)"Color in");
}

void ImportOptionsDialog::onColorInButtonClicked() {
	setButtonBackground(ui->colorInButton, (char*)"Color in");
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

void ImportOptionsDialog::onButtonBoxAccepted() {
	m_frameSize = QSize(ui->spriteWidth->value(), ui->spriteHeight->value());
	m_transparencyIn = QColor(ui->colorInButton->palette().color(QPalette::Button));
	m_transparencyOut = QColor(ui->colorOutButton->palette().color(QPalette::Button));
	m_removeDuplicates = ui->noDupsCheckBox;
	accept();
}

void ImportOptionsDialog::onButtonBoxRejected() {
	reject();
}
