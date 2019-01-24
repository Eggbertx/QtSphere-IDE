#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>

#include "formats/mapfile.h"

class MapView : public QGraphicsView {
	Q_OBJECT
	public:
		explicit MapView(QWidget *parent = nullptr);
		~MapView();
		bool loadMap(QString filename);
		bool attachMap(MapFile* map);
		MapFile::layer *getLayer(int index);
		bool openFile(QString filename);
		MapFile* attachedMap();
		QPoint widgetToMapPos(QPoint pos);
		QPoint widgetToMapPos(int x, int y);
		QPoint mapToWidgetPos(QPoint pos);
		QPoint mapToWidgetPos(int x, int y);
		void setDrawSize(int size);
		int getDrawSize();

	signals:

	public slots:

	protected:
		void mouseMoveEvent(QMouseEvent* event);
		void mousePressEvent(QMouseEvent* event);
		void leaveEvent(QEvent *event);

	private:
		QGraphicsItemGroup* createPointer(int size);
		QGraphicsScene* mapScene;
		typedef struct rmp_layer {
			QList<QGraphicsPixmapItem*> tiles;
			int width;
			int height;
		}rmp_layer;
		QList<rmp_layer> layers;
		MapFile* mapFile;
		int drawSize = 1;
		QGraphicsItemGroup* pointerGroup;
		QPoint pointerPosition;
		QString mapStatusFormat;

};

#endif // MAPVIEW_H
