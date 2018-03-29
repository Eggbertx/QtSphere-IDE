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
        void setTheme(QString theme);
        QString configDirectory;
        QString configPath;
		QStringList projectPaths;
        QStringList recentFiles;
		QString lastOpenedProject;
		QString theme;
		QString whichEngine;
        bool useWine;
        QString legacyDir;
        QString legacyConfigExe;
        QString legacyEnginePath;

	private:
        QFile* jsonFile;

		QString jsonString;
		QJsonDocument jsonDoc;
		QJsonObject jsonObj;
		void createDefaultConfig();
};

extern Config ideConfig;

#endif // CONFIG_H
