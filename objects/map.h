#ifndef MAP_H
#define MAP_H

#include <QObject>

class Map : public QObject {
	Q_OBJECT

	public:
		explicit Map(QObject *parent = 0);
		bool running();
		QString getMapName();
		void setMapName(QString name);

		bool mapOrigin();

	signals:

	public slots:

	private:
		QString mapName;
};

#endif
