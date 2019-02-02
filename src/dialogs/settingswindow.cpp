#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPixmap>
#include <QPushButton>
#include <QSettings>
#include <QStyle>

#include "mainwindow.h"
#include "dialogs/settingswindow.h"
#include "ui_settingswindow.h"
#include "util.h"

SettingsWindow::SettingsWindow(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsWindow) {
	ui->setupUi(this);
	ui->minisphereDir_btn->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	ui->legacySphereDir_btn->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
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
	if(settings.value("whichEngine", "minisphere") == "legacy") {
		ui->whichEngine_combo->setCurrentIndex(1);
	} else {
		ui->whichEngine_combo->setCurrentIndex(0);
	}
	ui->minisphereDir_txt->setText(settings.value("minisphereDir").toString());
	ui->legacySphereDir_txt->setText(settings.value("legacySphereDir").toString());
	m_mapCursorColor = settings.value("mapCursorColor", "#0080ff").toString();
	QColor col(m_mapCursorColor);
	if(col.isValid()) ui->mapCursorCol_btn->setColor(col);
	connect(ui->mapCursorCol_btn,SIGNAL(colorChanged(QColor)),this,SLOT(mapCursorColChanged(QColor)));
	connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(onOk()));
	connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(onApply()));
	connect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(onCancel()));
}

SettingsWindow::~SettingsWindow() {
	disconnect(ui->mapCursorCol_btn,SIGNAL(colorChanged(QColor)),this,SLOT(mapCursorColChanged(QColor)));
	disconnect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(onOk()));
	disconnect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(onApply()));
	disconnect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(onCancel()));
	delete ui;
}

void SettingsWindow::onOk() {
	m_currentTheme = m_newTheme;
	saveSettings();
	accept();
}

void SettingsWindow::onApply() {
	m_currentTheme = m_newTheme;
	saveSettings();
}

void SettingsWindow::onCancel() {
	MainWindow::instance()->setTheme(m_currentTheme);
	reject();
}

void SettingsWindow::on_minisphereDir_btn_clicked() {
	m_minisphereDir = QFileDialog::getExistingDirectory(this, "Open Directory");
	ui->minisphereDir_txt->setText(m_minisphereDir);
}

void SettingsWindow::on_legacySphereDir_btn_clicked() {
	m_legacySphereDir = QFileDialog::getExistingDirectory(this, "Open Directory");
	ui->legacySphereDir_txt->setText(m_legacySphereDir);
}

void SettingsWindow::on_themeCombo_currentIndexChanged(const QString &newtext) {
	if(newtext == "Default") m_newTheme = "";
	else if(newtext == "Dark") m_newTheme = ":/text/dark.qss";
	MainWindow::instance()->setTheme(m_newTheme);
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
	if(ui->whichEngine_combo->currentText() == "Use Sphere 1.x (legacy)") {
		settings.setValue("whichEngine", "legacy");
	} else {
		settings.setValue("whichEngine", "minisphere");
	}
	MainWindow::instance()->setEngine(settings.value("whichEngine").toString());
	settings.setValue("minisphereDir", ui->minisphereDir_txt->text());
	settings.setValue("legacySphereDir", ui->legacySphereDir_txt->text());
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


