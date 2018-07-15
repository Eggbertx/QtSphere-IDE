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
	qDebug().noquote() << "Loading config file from " << this->configPath;

	if(!QFile(this->configPath).exists())
		this->createDefaultConfig();

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
}

void Config::setTheme(QString theme = "customstyle.qss", bool inConfigDir) {
	QFile* styleFile;
	if(inConfigDir) styleFile = new QFile(this->configDirectory + theme);
	else styleFile = new QFile(theme);
	if(!styleFile->open(QFile::ReadOnly)) {
		QString errorText = "Failed to open stylesheet (" + theme + "):" + styleFile->errorString();
		MainWindow::instance()->console(errorText, 1);
		qDebug() << errorText;
		return;
	}

	QString stylesheet = QLatin1String(styleFile->readAll());
	styleFile->close();
	delete styleFile;
	qApp->setStyleSheet(stylesheet);
}

void Config::save() {

}

void Config::createDefaultConfig() {
	if(QFile::exists(this->configDirectory)) return;
	QDir().mkdir(QDir::homePath() + "/.QtSphereIDE");
	QFile::copy(":/text/QtSphereIDE.json", this->configDirectory);
}
