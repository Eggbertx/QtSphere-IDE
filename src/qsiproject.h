#ifndef QSIPROJECT_H
#define QSIPROJECT_H

#include <QFile>
#include <QIcon>
#include <QObject>

class QSIProject : public QObject {
	Q_OBJECT
	Q_ENUMS(Compiler)
	public:
		explicit QSIProject(QObject *parent = nullptr);
		bool open(QString path);

		QString getName();
		void setName(QString name);
		QString getAuthor();
		void setAuthor(QString author);
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
		QString getBuidlDir();
		void setBuildDir(QString dir);
		QString getMainScript();
		void setMainScript(QString path);
		QString getPath(bool projectFile);
		void setPath(QString path, bool projectFile = false);
		QString getCompiler();
		void setCompiler(QString compiler);
		QIcon getIcon();
		enum Compiler { Vanilla, Cell };

	private:
		bool readSSProj(QFile* projectFile);
		bool readCellscript(QFile* projectFile);
		bool readSGM(QFile* projectFile);
		QString m_name;
		QString m_author;
		int m_width;
		int m_height;
		int m_apiLevel;
		int m_version;
		QString m_saveID;
		QString m_summary;
		QString m_buildDir;
		QString m_script;
		QString m_path;
		QString m_projectPath;
		QString m_compiler;
};

#endif // QSIPROJECT_H
