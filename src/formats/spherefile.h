#ifndef SPHEREFILE_H
#define SPHEREFILE_H

#include <QObject>
#include <QFile>
#include <QTextEdit>

class SphereFile : public QObject {
	Q_OBJECT
	Q_ENUMS(Type)
	public:
		explicit SphereFile(QObject *parent = nullptr);
		~SphereFile();
		virtual bool save(QString filename);
		virtual bool open(QString filename, QIODevice::OpenMode flags);
		bool isOpen();
		void close();
		virtual void createNew();
		QString fileExtension();
		QString fileName();
		QString getBaseName();
		QString filePath();
		enum Type { Font, Map, Package, Spriteset, TilesetFile, WindowStyle }; // binary types
	signals:

	public slots:

	protected:
		QString readNextString();
		QFile* m_file;
		QString m_filename;
};

#endif // SPHEREFILE_H
