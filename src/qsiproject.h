#ifndef QSIPROJECT_H
#define QSIPROJECT_H

#include <QObject>

class QSIProject : public QObject {
	Q_OBJECT
	Q_ENUMS(Compiler)
	public:
		explicit QSIProject(QString path, QObject *parent = 0);
		QString getPath();
		QString name;
		QString author;
		int width;
		int height;
		QString summary;
		QString buildDir;
		QString script;
		QString getCompiler();
		void setCompiler(QString set);
		enum Compiler { Vanilla, Cell };
	signals:

	public slots:

	private:
		QString path;
		QString compler;
};

#endif // QSIPROJECT_H
