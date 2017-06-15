#ifndef QSIFILE_H
#define QSIFILE_H
/*
 * QSIFile is an abstract object to be inherited by all of Sphere/miniSphere's filetypes:
 *
 *
 */
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

	//explicit SphereFile(QString fn, QTextEdit *parentWidget);
	//explicit SphereFile(QFile *file = nullptr);
	//explicit SphereFile(QString fn);

    QString fileExtension();
    QString* fileName();
    QString getBaseName();
	QWidget* getWidget();
	void save();

	void readFile(QString filename);
	void readFile(QFile f);
	QString filePath();
signals:

public slots:

private:
	QFile* file;
	QWidget* widget;
};

#endif // QSIFILE_H
