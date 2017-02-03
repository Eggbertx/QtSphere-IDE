#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

class Config {
	public:
		Config();
		void loadConfig();
		QString configPath;
		QStringList projectPaths;
		QString lastOpenedProject;
		QString theme;
		QString whichEngine;

	private:
		QFile* jsonFile;
		QString jsonString;
		QJsonDocument jsonDoc;
		QJsonObject jsonObj;
		void createDefaultConfig();
};

extern Config ideConfig;

#endif // CONFIG_H
