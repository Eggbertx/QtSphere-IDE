#include <QStyle>
#include <QFileDialog>
#include <QDebug>
#include <QLineEdit>

#include "mainwindow.h"
#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) : QDialog(parent),
	ui(new Ui::SettingsWindow) {
	ui->setupUi(this);
	ui->selLegacyDir->setIcon(this->style()->standardIcon(QStyle::SP_DirOpenIcon));
}

SettingsWindow::~SettingsWindow() {
	delete ui;
}

void SettingsWindow::on_cancelButton_clicked() {
	this->close();
}

void SettingsWindow::on_selLegacyDir_clicked() {
	this->legacyDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"/home", QFileDialog::ShowDirsOnly);
	ui->legacyDir_txt->setText(this->legacyDir);

}
