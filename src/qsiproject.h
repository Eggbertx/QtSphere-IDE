#ifndef QSIPROJECT_H
#define QSIPROJECT_H

#include <QObject>

class QSIProject : public QObject {
	Q_OBJECT
	public:
		explicit QSIProject(QString path, QObject *parent = 0);
		QString getPath();
		QString name;
		QString author;
		QString resolution;
		QString summary;
		QString buildDir;
		QString compler;
		QString gitURL;
	signals:

	public slots:

	private:
		const QString path;
};

#endif // QSIPROJECT_H
