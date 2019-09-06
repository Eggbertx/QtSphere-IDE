#ifndef DRAWCOMMAND_H
#define DRAWCOMMAND_H

#include <QUndoCommand>

#include "widgets/map/mapview.h"

class MapDrawCommand : public QUndoCommand {
	public:
		enum { Id = 131};
		MapDrawCommand(MapView* view, QList<QPoint>points, int tileIndex, QWidget* parent);
		void undo() override;
		void redo() override;
		bool mergeWith(const QUndoCommand *command) override;

	private:
		MapView* m_mapView;
		QList<QPoint> m_points;
		int m_tileIndex;
};

#endif // DRAWCOMMAND_H
