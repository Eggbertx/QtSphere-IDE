#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <QGraphicsScene>
#include <QTableWidgetItem>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

#include "widgets/sphereeditor.h"
#include "formats/mapfile.h"
#include "widgets/map/mapview.h"
#include "widgets/wrappedgraphicsview.h"

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
		void on_layersTable_customContextMenuRequested(const QPoint &pos);
		void layerPropertiesRequested(bool triggered);
		void on_layersTable_itemChanged(QTableWidgetItem* item);
		void on_layersTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

	private:
		void createUndoActions();
		Ui::MapEditor *ui;
		QMenu* m_layerMenu;

		// menubar tools
		QToolBar* m_menuBar;
		QMenu* m_pencilMenu;
		QAction* m_pencil1;
		QAction* m_pencil3;
		QAction* m_pencil5;
		QAction* m_pencilTool;
		QAction* m_lineTool;
		QAction* m_fillTool;
		QAction* m_dropperTool;
		int m_currentTool = MapEditor::Pencil;
		QAction* m_gridTool;

		// QActions for the undo stack
		QAction* m_drawTiles;
		QAction* m_rectTool;
		QAction* m_fillTiles;
		QAction* m_hideLayer;
		QAction* m_showLayer;
		QAction* m_deleteLayer;
		QAction* m_addLayer;
		QAction* m_renameLayer;
		QAction* m_changeLayerProperties;
};

#endif // MAPEDITOR_H
