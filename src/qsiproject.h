#ifndef QSIPROJECT_H
#define QSIPROJECT_H

#include <QFile>
#include <QIcon>
#include <QObject>
#include <QTextStream>

class QSIProject : public QObject {
	Q_OBJECT

	public:
		enum ProjectFileFormat {
			UnknownProjectType = -1,
			SSProject,
			Cellscript_js,
			Cellscript_mjs,
			Cellscript_cjs,
			SGM
		};
		Q_ENUM(ProjectFileFormat)
		explicit QSIProject(QObject *parent = nullptr);
		~QSIProject();

		bool open(QString path);
		bool save();
		QString getName();
		void setName(QString name);
		QString getAuthor();
		void setAuthor(QString author);
		QString getResolutionString();
		int getWidth();
		void setWidth(int width);
		int getHeight();
		void setHeight(int height);
		void setSize(int width, int height);
		int getAPILevel();
		void setAPILevel(int level);
		int getVersion();
		void setVersion(int version);
		QString getSaveID();
		void setSaveID(QString id);
		QString getSummary();
		void setSummary(QString summary);
		QString getBuildDir();
		void setBuildDir(QString dir);
		QString getMainScript();
		void setMainScript(QString path);
		QString getPath(bool projectFile);
		void setPath(QString path, bool projectFile = false);
		QString getCompiler();
		void setCompiler(QString compiler);
		QIcon getIcon();
		ProjectFileFormat getProjectFormat();

	private:
		QString getCellscriptStringValue(QString cellscriptStr, QString key, QString defaultValue = "");
		int getCellscriptIntValue(QString cellscriptStr, QString key, int defaultValue = -1);
		bool prepareProjectFile(QFile* projectFile);
		bool readSSProj(QFile* projectFile);
		bool readCellscript(QFile* projectFile);
		bool readSGM(QFile* projectFile);
		QString m_name;
		QString m_author;
		int m_width;
		int m_height;
		int m_apiLevel;
		int m_version;
		ProjectFileFormat m_projectFileFormat;
		QString m_projectFilePath;
		QString m_saveID;
		QString m_summary;
		QString m_buildDir;
		QString m_script;
		QString m_projectDir;
		QString m_compiler;
};

#endif // QSIPROJECT_H
