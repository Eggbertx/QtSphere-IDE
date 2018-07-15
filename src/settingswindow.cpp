#include <QStyle>
#include <QFileDialog>
#include <QDebug>
#include <QLineEdit>

#include "mainwindow.h"
#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "util.h"

SettingsWindow::SettingsWindow(QWidget *parent) : QDialog(parent),
	ui(new Ui::SettingsWindow) {
	ui->setupUi(this);
	this->mainWindow = static_cast<MainWindow*>(parent);
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

void SettingsWindow::on_themeCombo_currentIndexChanged(const QString &newtext) {
	if(newtext == "Default") qApp->setStyleSheet("");
	if(newtext == "Dark") this->mainWindow->config->setTheme(":/text/dark.qss");
}

void SettingsWindow::on_okButton_clicked() {
	this->mainWindow->config->theme = ui->themeCombo->currentText();

}
