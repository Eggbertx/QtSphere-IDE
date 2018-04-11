#include <QDesktopServices>
#include <QFile>

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent): QDialog(parent), ui(new Ui::AboutDialog) {
	ui->setupUi(this);
}

AboutDialog::~AboutDialog() {
	delete ui;
}

void AboutDialog::on_closeAboutDialog_clicked() {
	this->close();
}
