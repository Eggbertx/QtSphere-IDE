#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "config.h"
#include "mainwindow.h"

Config::Config() {
	this->configDirectory = QDir::homePath() + "/.QtSphereIDE/";
	this->configPath = this->configDirectory + "QtSphereIDE.json";
	this->jsonFile = new QFile(this->configPath);
}

void Config::loadConfig() {
	qDebug().noquote() << QString("Loading config file from ") + this->configPath + QString("/.QtSphereIDE...");

	if(!QFile(this->configPath).exists())
		createDefaultConfig();

	jsonFile->setFileName(this->configPath);
	jsonFile->open(QIODevice::ReadWrite | QIODevice::Text);
	jsonString = jsonFile->readAll();
	jsonFile->close();

	jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
	jsonObj = jsonDoc.object();

	this->theme = jsonObj["theme"].toString();

	QJsonArray projectArray = jsonObj["projectPaths"].toArray();
	for(int p = 0; p < projectArray.size(); p++) {
		this->projectPaths.append(projectArray[p].toString());
	}
	qDebug().noquote() << jsonObj["legacySphere"].toObject()["legacySphereDir"].toString();
}

void Config::setTheme(QString theme = "stylesheet.qss") {
	QFile styleFile(this->configDirectory + theme);
	if(!styleFile.open(QFile::ReadOnly)) {
		MainWindow::instance()->console("Failed to open stylesheet (" + theme + "):" + styleFile.errorString(), 1);
		return;
	}
	QString stylesheet = QLatin1String(styleFile.readAll());
	styleFile.close();
	qApp->setStyleSheet(stylesheet);
}

void Config::createDefaultConfig() {
	QDir().mkdir(QDir::homePath() + "/.QtSphereIDE");
	QFile* defaultConfig = new QFile(":/text/QtSphereIDE.json");
	defaultConfig->copy(this->configPath);
	// open settings dialog
	defaultConfig->close();
}
