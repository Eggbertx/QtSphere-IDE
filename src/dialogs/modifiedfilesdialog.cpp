#include <QTreeWidgetItem>

#include "dialogs/modifiedfilesdialog.h"
#include "ui_modifiedfilesdialog.h"

ModifiedFilesDialog::ModifiedFilesDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ModifiedFilesDialog) {
	ui->setupUi(this);
}

ModifiedFilesDialog::~ModifiedFilesDialog() {
	delete ui;
}

void ModifiedFilesDialog::on_cancelButton_clicked() {
	m_result = 0;
	close();
}

void ModifiedFilesDialog::on_discardAllButton_2_clicked() {
	m_result = 1;
	close();
}

void ModifiedFilesDialog::addModifiedItem(SphereFile *item) {
	QTreeWidgetItem *twi = new QTreeWidgetItem(ui->treeWidget);
	QString filename = item->fileName();
	twi->setText(0,filename);
	ui->treeWidget->insertTopLevelItem(0,twi);
}

void ModifiedFilesDialog::on_saveAllButton_clicked() {
	m_result = 2;
	close();
}

int ModifiedFilesDialog::getResult() {
	return m_result;
}
