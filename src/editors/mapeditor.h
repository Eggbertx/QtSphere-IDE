#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <QGraphicsScene>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

#include "sphereeditor.h"
#include "formats/mapfile.h"
#include "mapview.h"
#include "wrappedgraphicsview.h"

namespace Ui {
	class MapEditor;
}

class MapEditor : public SphereEditor {
	Q_OBJECT
	Q_ENUMS(Tools)
	public:
		explicit MapEditor(QWidget *parent = nullptr);
		~MapEditor();
		bool openFile(QString filename);
		bool attach(MapFile* attachedMap);
		enum Tools { Pencil, Line, Rectangle, Fill, Select };

	private slots:
		void on_layersTable_cellClicked(int row, int column);
		void setPencilSize(QAction* size);
		void setCurrentTool(QAction* tool);
		void setTileIndex(int tile);

	private:
		Ui::MapEditor *ui;
		int currentTool = MapEditor::Pencil;
		MapView* mapView;
		QVBoxLayout* tilesetLayout;
		WrappedGraphicsView* tilesetView;
		QToolBar* menuBar;
		QMenu* pencilMenu;
};

#endif // MAPEDITOR_H
