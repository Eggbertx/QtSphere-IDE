#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPixmap>
#include <QSettings>
#include <QStyle>

#include "mainwindow.h"
#include "dialogs/settingswindow.h"
#include "ui_settingswindow.h"
#include "util.h"

SettingsWindow::SettingsWindow(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsWindow) {
	ui->setupUi(this);
	ui->legacyDir_btn->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	m_currentTheme = MainWindow::instance()->getTheme();
	m_newTheme = m_currentTheme;
	QSettings settings;
	int size = settings.beginReadArray("projectDirs");
	for(int i = 0; i < size; ++i) {
		settings.setArrayIndex(i);
		QListWidgetItem* item = new QListWidgetItem(settings.value("directory").toString(), ui->projectDirsList);
		item->setFlags(item->flags()|Qt::ItemIsEditable);
		ui->projectDirsList->addItem(item);
	}
	settings.endArray();
	m_mapCursorColor = settings.value("mapCursorColor", "#0080ff").toString();
	QColor col(m_mapCursorColor);
	if(col.isValid()) ui->mapCursorCol_btn->setColor(col);
	connect(ui->mapCursorCol_btn,SIGNAL(colorChanged(QColor)),this,SLOT(mapCursorColChanged(QColor)));
}

SettingsWindow::~SettingsWindow() {
	disconnect(ui->mapCursorCol_btn,SIGNAL(colorChanged(QColor)),this,SLOT(mapCursorColChanged(QColor)));
	delete ui;
}

void SettingsWindow::on_cancelButton_clicked() {
	MainWindow::instance()->setTheme(m_currentTheme);
	reject();
}

void SettingsWindow::on_legacyDir_btn_clicked() {
	m_legacyDir = QFileDialog::getExistingDirectory(this, "Open Directory");
	ui->legacyDir_txt->setText(m_legacyDir);
}

void SettingsWindow::on_themeCombo_currentIndexChanged(const QString &newtext) {
	if(newtext == "Default") m_newTheme = "";
	else if(newtext == "Dark") m_newTheme = ":/text/dark.qss";
	MainWindow::instance()->setTheme(m_newTheme);

}

void SettingsWindow::on_okButton_clicked() {
	m_currentTheme = m_newTheme;
	saveSettings();
	accept();
}

void SettingsWindow::on_applyButton_clicked() {
	m_currentTheme = m_newTheme;
	saveSettings();
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
	settings.setValue("theme", m_currentTheme);
	if(ui->whichEngine_combo->currentText() == "Use Sphere 1.x (legacy)")
		settings.setValue("whichEngine", "legacy");
	else settings.setValue("whichEngine", "minisphere");
	settings.setValue("legacyEnginePath", ui->legacyDir_txt->text());
	settings.setValue("mapCursorColor", m_mapCursorColor);
}

void SettingsWindow::on_addDirButton_clicked() {
	QListWidgetItem* newItem = new QListWidgetItem("", ui->projectDirsList);
	newItem->setFlags(newItem->flags()|Qt::ItemIsEditable);
	ui->projectDirsList->addItem(newItem);
	ui->projectDirsList->setCurrentRow(ui->projectDirsList->count()-1);
}

void SettingsWindow::on_removeDirButton_clicked() {
	ui->projectDirsList->takeItem(ui->projectDirsList->currentRow());
}

void SettingsWindow::mapCursorColChanged(QColor color) {
	m_mapCursorColor = color.name();
}
