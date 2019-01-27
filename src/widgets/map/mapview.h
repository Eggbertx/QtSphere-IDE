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
		~MapView() override;
		bool loadMap(QString filename);
		bool openFile(QString filename);
		MapFile* attachedMap();
		bool attachMap(MapFile* map);
		MapFile::layer *getLayer(int index);
		void setLayerVisible(int layer, bool visible);
		bool toggleLayerVisible(int layer);
		QPoint widgetToMapPos(QPoint pos);
		QPoint widgetToMapPos(int x, int y);
		QPoint mapToWidgetPos(QPoint pos);
		QPoint mapToWidgetPos(int x, int y);
		void setDrawSize(int size);
		int getDrawSize();
		void setCurrentTile(int tile);

	signals:

	public slots:

	protected:
		void mouseMoveEvent(QMouseEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;
		void leaveEvent(QEvent *event) override;

	private:
		QGraphicsItemGroup* createPointer(int size);
		QRect pointerRect(bool tiles);
		void drawTile(int index);
		QGraphicsScene* mapScene;
		typedef struct rmp_layer {
			QList<QGraphicsPixmapItem*> tiles;
			int width;
			int height;
		}rmp_layer;
//		QList<rmp_layer> layers;
		MapFile* mapFile;
		int drawSize = 1;
		QGraphicsItemGroup* pointerGroup;
		QPoint pointerPosition;
		QString mapStatusFormat;
		bool drawing = false;
		int currentTile = 0;

};

#endif // MAPVIEW_H
