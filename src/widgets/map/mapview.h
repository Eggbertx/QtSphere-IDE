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
		void setCurrentLayer(int layer);
		int getCurrentLayer();
		bool toggleLayerVisible(int layer);
		void deleteLayer(int layer);

		QPoint widgetToMapPos(QPoint pos);
		QPoint widgetToMapPos(int x, int y);
		QPoint mapToWidgetPos(QPoint pos);
		QPoint mapToWidgetPos(int x, int y);

		void setDrawSize(int size);
		int getDrawSize();
		void setCurrentTile(int tile);

		void setCurrentTool(int tool);
		void setGridVisible(bool visible);
		bool isGridVisible();

	public slots:

	protected:
		void mouseMoveEvent(QMouseEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;
		void leaveEvent(QEvent *event) override;

	private:
		void updatePointer(int size);
		void updateGrid();
		QRect pointerRect(bool tiles);
		void drawTile(int index);

		QGraphicsScene* m_mapScene;
		MapFile* m_mapFile;
		int m_drawSize = 1;
		QGraphicsItemGroup* m_gridGroup;
		bool m_gridVisible;
		QGraphicsItemGroup* m_pointerGroup;
		QPoint m_pointerPosition;
		QString m_mapStatusFormat;
		bool m_drawing = false;
		int m_currentTile = 0;
		int m_currentLayer = 0;
};

#endif // MAPVIEW_H
