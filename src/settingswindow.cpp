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
}

SettingsWindow::~SettingsWindow() {
	delete ui;
}

void SettingsWindow::on_cancelButton_clicked() {
	this->close();
}

void SettingsWindow::on_legacyDir_btn_clicked() {
	this->legacyDir = QFileDialog::getExistingDirectory(this, "Open Directory");
	ui->legacyDir_txt->setText(this->legacyDir);
}
