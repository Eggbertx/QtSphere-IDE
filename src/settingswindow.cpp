#include <QStyle>
#include <QFileDialog>
#include <QDebug>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSettings>

#include "mainwindow.h"
#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "util.h"

SettingsWindow::SettingsWindow(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsWindow) {
	ui->setupUi(this);
	ui->legacyDir_btn->setIcon(this->style()->standardIcon(QStyle::SP_DialogOpenButton));
	this->currentTheme = MainWindow::instance()->getTheme();
	this->newTheme = this->currentTheme;
	QSettings settings;
	int size = settings.beginReadArray("projectDirs");
	for(int i = 0; i < size; ++i) {
		settings.setArrayIndex(i);
		QListWidgetItem* item = new QListWidgetItem(settings.value("directory").toString(), ui->projectDirsList);
		item->setFlags(item->flags()|Qt::ItemIsEditable);
		ui->projectDirsList->addItem(item);
	}
	settings.endArray();
}

SettingsWindow::~SettingsWindow() {
	delete ui;
}

void SettingsWindow::on_cancelButton_clicked() {
	MainWindow::instance()->setTheme(this->currentTheme);
	this->reject();
}

void SettingsWindow::on_legacyDir_btn_clicked() {
	this->legacyDir = QFileDialog::getExistingDirectory(this, "Open Directory");
	ui->legacyDir_txt->setText(this->legacyDir);
}

void SettingsWindow::on_themeCombo_currentIndexChanged(const QString &newtext) {
	if(newtext == "Default") this->newTheme = "";
	else if(newtext == "Dark") this->newTheme = ":/text/dark.qss";
	MainWindow::instance()->setTheme(this->newTheme);

}

void SettingsWindow::on_okButton_clicked() {
	this->currentTheme = this->newTheme;
	this->saveSettings();
	this->accept();
}

void SettingsWindow::on_applyButton_clicked() {
	this->currentTheme = this->newTheme;
	this->saveSettings();
}

void SettingsWindow::saveSettings() {
	QSettings settings;
	int numDirs = ui->projectDirsList->count();
	settings.remove("projectDirs");
	settings.beginWriteArray("projectDirs");
	for(int d = 0; d < numDirs; d++) {
		settings.setArrayIndex(d);
		QString directory = ui->projectDirsList->item(d)->text();
		if(directory != "" && !ui->projectDirsList->item(d)->isHidden()) {
			settings.setValue("directory", directory);
		}
	}
	settings.endArray();
	settings.setValue("theme", this->currentTheme);
	if(ui->whichEngine_combo->currentText() == "Use Sphere 1.x (legacy)")
		settings.setValue("whichEngine", "legacy");
	else settings.setValue("whichEngine", "minisphere");
	settings.setValue("legacyEnginePath", ui->legacyDir_txt->text());
}

void SettingsWindow::on_addDirButton_clicked() {
	QListWidgetItem* newItem = new QListWidgetItem("", ui->projectDirsList);
	newItem->setFlags(newItem->flags()|Qt::ItemIsEditable);
	ui->projectDirsList->addItem(newItem);
	ui->projectDirsList->setCurrentRow(ui->projectDirsList->count()-1);
}

void SettingsWindow::on_removeDirButton_clicked() {
	QListWidgetItem* cur = ui->projectDirsList->currentItem();
	if(cur == nullptr) return;
	//ui->projectDirsList->removeItemWidget(cur);
	ui->projectDirsList->takeItem(ui->projectDirsList->currentRow());
	delete cur;
}
