#ifndef MAPEDITOR_H
#define MAPEDITOR_H

#include <QGraphicsScene>
#include <QWidget>
#include "sphereeditor.h"
#include "formats/mapfile.h"

namespace Ui {
	class MapEditor;
}

class MapEditor : public SphereEditor {
	Q_OBJECT

	public:
		explicit MapEditor(QWidget *parent = nullptr);
		~MapEditor();
		bool openFile(QString filename);
		bool attach(MapFile* attachedMap);

	private slots:
		void on_layersTable_cellClicked(int row, int column);

	private:
		Ui::MapEditor *ui;
		MapFile* mapFile;
		QGraphicsScene* mapScene;
		QGraphicsScene* tilesScene;

};

#endif // MAPEDITOR_H
