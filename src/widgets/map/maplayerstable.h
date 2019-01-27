#ifndef MAPLAYERSTABLE_H
#define MAPLAYERSTABLE_H

#include <QTableWidget>
#include "formats/mapfile.h"

class MapLayersTable : public QTableWidget {
	public:
		explicit MapLayersTable(QWidget *parent = nullptr);
		void attachMap(MapFile* map);


	signals:
		void layerChanged(int index);

	protected:
		void mousePressEvent(QMouseEvent* event) override;

	private:
		MapFile* map;
		QMenu* layerMenu;

};

#endif // MAPLAYERSTABLE_H
