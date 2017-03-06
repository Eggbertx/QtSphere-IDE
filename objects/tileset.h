#ifndef TILESET_H
#define TILESET_H

#include <QObject>

class Tileset : public QObject {
	Q_OBJECT
	public:
		explicit Tileset(QObject *parent = 0);

		int getDelay(int index);
		void setDelay(int index);
		void getImage(int index);

	signals:

	public slots:
};

#endif // TILESET_H
