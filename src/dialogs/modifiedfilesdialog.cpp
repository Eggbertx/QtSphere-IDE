#include <QTreeWidgetItem>

#include "dialogs/modifiedfilesdialog.h"
#include "ui_modifiedfilesdialog.h"

ModifiedFilesDialog::ModifiedFilesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ModifiedFilesDialog) {
	ui->setupUi(this);
	connect(ui->cancelBtn, &QPushButton::clicked, this, &ModifiedFilesDialog::onCancelButtonClicked);
	connect(ui->discardAllBtn, &QPushButton::clicked, this, &ModifiedFilesDialog::onDiscardAllButtonClicked);
	connect(ui->saveAllBtn, &QPushButton::clicked, this, &ModifiedFilesDialog::onSaveAllButtonClicked);
}

ModifiedFilesDialog::~ModifiedFilesDialog() {
	disconnect(ui->cancelBtn, &QPushButton::clicked, this, &ModifiedFilesDialog::onCancelButtonClicked);
	disconnect(ui->discardAllBtn, &QPushButton::clicked, this, &ModifiedFilesDialog::onDiscardAllButtonClicked);
	disconnect(ui->saveAllBtn, &QPushButton::clicked, this, &ModifiedFilesDialog::onSaveAllButtonClicked);
	delete ui;
}

void ModifiedFilesDialog::onCancelButtonClicked() {
	m_result = 0;
	close();
}

void ModifiedFilesDialog::onDiscardAllButtonClicked() {
	m_result = 1;
	close();
}

void ModifiedFilesDialog::addModifiedItem(SphereFile *item) {
	QTreeWidgetItem *twi = new QTreeWidgetItem(ui->treeWidget);
	QString filename = item->fileName();
	twi->setText(0,filename);
	ui->treeWidget->insertTopLevelItem(0,twi);
}

void ModifiedFilesDialog::onSaveAllButtonClicked() {
	m_result = 2;
	close();
}

int ModifiedFilesDialog::getResult() {
	return m_result;
}
