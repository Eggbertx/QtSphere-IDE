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
		void onPencilClick();
		void setPencilSize(QAction* size);
		void setCurrentTool(QAction* tool);
		void setTileIndex(int tile);
		void on_layersTable_customContextMenuRequested(const QPoint &pos);
		void layerPropertiesRequested(bool triggered);
		void on_layersTable_itemChanged(QTableWidgetItem* item);

		void on_layersTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

	private:
		Ui::MapEditor *ui;
		int m_currentTool = MapEditor::Pencil;
//		QVBoxLayout* m_tilesetLayout;
//		WrappedGraphicsView* m_tilesetView;
		QToolBar* m_menuBar;
		QToolButton* m_pencilMenuButton;
		QMenu* m_pencilMenu;
		QMenu* m_layerMenu;
};

#endif // MAPEDITOR_H
