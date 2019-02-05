#include <QDesktopServices>
#include <QFile>

#include "dialogs/aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent): QDialog(parent), ui(new Ui::AboutDialog) {
	ui->setupUi(this);
	ui->label->setText(ui->label->text().replace("{VERSION}", VERSION));
}

AboutDialog::~AboutDialog() {
	delete ui;
}

void AboutDialog::on_closeAboutDialog_clicked() {
	close();
}
