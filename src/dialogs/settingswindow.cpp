#include <QColorDialog>
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
	ui->neosphereDir_btn->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
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
	if(settings.value("whichEngine", "neosphere") == "legacy") {
		ui->whichEngine_combo->setCurrentIndex(1);
	} else {
		ui->whichEngine_combo->setCurrentIndex(0);
	}
	ui->neosphereDir_txt->setText(settings.value("neosphereDir").toString());
	ui->legacySphereDir_txt->setText(settings.value("legacySphereDir").toString());
	m_mapCursorColor = settings.value("mapCursorColor", "#0080ff").toString();
	m_gridColor = settings.value("gridColor", "#000000").toString();
	QColor col(m_mapCursorColor);
	if(col.isValid()) ui->mapCursorCol_btn->setColor(col);
	col = QColor::fromString(m_gridColor);
	if(col.isValid()) ui->gridColor_btn->setColor(col);
	connect(ui->mapCursorCol_btn,SIGNAL(colorChanged(QColor)),this,SLOT(mapCursorColChanged(QColor)));
	connect(ui->gridColor_btn, SIGNAL(colorChanged(QColor)), this, SLOT(gridColorChanged(QColor)));
	connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(onOk()));
	connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(onApply()));
	connect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(onCancel()));
	connect(ui->neosphereDir_btn, &QToolButton::clicked, this, &SettingsWindow::onNeosphereDirButtonClicked);
	connect(ui->legacySphereDir_btn, &QToolButton::clicked, this, &SettingsWindow::onLegacySphereDirButtonClicked);
	connect(ui->themeCombo, &QComboBox::currentTextChanged, this, &SettingsWindow::onThemeComboCurrentTextChanged);
	connect(ui->addDirButton, &QToolButton::clicked, this, &SettingsWindow::onAddDirButtonClicked);
	connect(ui->removeDirButton, &QToolButton::clicked, this, &SettingsWindow::onRemoveDirButtonClicked);
	connect(ui->browseDirButton, &QToolButton::clicked, this, &SettingsWindow::onBrowseDirButtonClicked);
}

SettingsWindow::~SettingsWindow() {
	disconnect(ui->mapCursorCol_btn,SIGNAL(colorChanged(QColor)),this,SLOT(mapCursorColChanged(QColor)));
	disconnect(ui->gridColor_btn, SIGNAL(colorChanged(QColor)), this, SLOT(gridColorChanged(QColor)));
	disconnect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(onOk()));
	disconnect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(onApply()));
	disconnect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(onCancel()));
	disconnect(ui->neosphereDir_btn, &QToolButton::clicked, this, &SettingsWindow::onNeosphereDirButtonClicked);
	disconnect(ui->legacySphereDir_btn, &QToolButton::clicked, this, &SettingsWindow::onLegacySphereDirButtonClicked);
	disconnect(ui->themeCombo, &QComboBox::currentTextChanged, this, &SettingsWindow::onThemeComboCurrentTextChanged);
	disconnect(ui->addDirButton, &QToolButton::clicked, this, &SettingsWindow::onAddDirButtonClicked);
	disconnect(ui->removeDirButton, &QToolButton::clicked, this, &SettingsWindow::onRemoveDirButtonClicked);
	disconnect(ui->browseDirButton, &QToolButton::clicked, this, &SettingsWindow::onBrowseDirButtonClicked);
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

void SettingsWindow::onNeosphereDirButtonClicked() {
	QString _m_neosphereDir = QFileDialog::getExistingDirectory(this, "Open Directory");
	if(_m_neosphereDir == "") return;
	m_neosphereDir = _m_neosphereDir;
	ui->neosphereDir_txt->setText(m_neosphereDir);
}

void SettingsWindow::onLegacySphereDirButtonClicked() {
	QString _m_legacySphereDir = QFileDialog::getExistingDirectory(this, "Open Directory", m_legacySphereDir);
	if(_m_legacySphereDir == "") return;
	m_legacySphereDir = _m_legacySphereDir;
	ui->legacySphereDir_txt->setText(m_legacySphereDir);
}

void SettingsWindow::onThemeComboCurrentTextChanged(const QString &newtext) {
	if(newtext == "Default")
		m_newTheme = "";
	else if(newtext == "Dark")
		m_newTheme = ":/text/dark.qss";
	else
		m_newTheme = "";
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
		settings.setValue("whichEngine", "neosphere");
	}
	MainWindow::instance()->setEngine(settings.value("whichEngine").toString());
	settings.setValue("neosphereDir", ui->neosphereDir_txt->text());
	settings.setValue("legacySphereDir", ui->legacySphereDir_txt->text());
	settings.setValue("mapCursorColor", m_mapCursorColor);
	settings.setValue("gridColor", m_gridColor);
}

void SettingsWindow::onAddDirButtonClicked() {
	QListWidgetItem* newItem = new QListWidgetItem("", ui->projectDirsList);
	newItem->setFlags(newItem->flags()|Qt::ItemIsEditable);
	ui->projectDirsList->addItem(newItem);
	ui->projectDirsList->setCurrentRow(ui->projectDirsList->count()-1);
}

void SettingsWindow::onRemoveDirButtonClicked() {
	ui->projectDirsList->takeItem(ui->projectDirsList->currentRow());
}

void SettingsWindow::mapCursorColChanged(QColor color) {
	m_mapCursorColor = color.name();
}

void SettingsWindow::gridColorChanged(QColor color) {
	m_gridColor = color.name();
}

void SettingsWindow::onBrowseDirButtonClicked() {
	int row = ui->projectDirsList->currentRow();
	if(row < 0) return;
	QString dir = QFileDialog::getExistingDirectory(this, "Open Directory");
	if(dir != "") {
		ui->projectDirsList->item(row)->setText(dir);
	}
}

