#ifndef SPHEREFILE_H
#define SPHEREFILE_H

#include <QObject>
#include <QFile>
#include <QTextEdit>

/* SphereFile is treated as a standard widget
 * to be inherited by other classes representing
 * Sphere filetypes. All Sphere subclasses implement
 * readFile, save, etc, along with any necessary
 * slots/signals
*/

class SphereFile : public QWidget {
    Q_OBJECT
public:
	explicit SphereFile(QWidget *parent = nullptr);
    explicit SphereFile(QFile *file = nullptr);
    explicit SphereFile(QString fn);

    QString fileExtension();
    QString fileName();
    QString getBaseName();
	QWidget* getWidget();
	void save();

	void readFile(QString filename);
    void readFile(QFile *f);
	QString filePath();
signals:

public slots:

private:
	QFile* file;
	QWidget* widget;
};

#endif // SPHEREFILE_H
