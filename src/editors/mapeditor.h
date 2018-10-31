#ifndef MAPEDITOR_H
#define MAPEDITOR_H

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
	private:
		Ui::MapEditor *ui;
		MapFile* mapFile;

};

#endif // MAPEDITOR_H
