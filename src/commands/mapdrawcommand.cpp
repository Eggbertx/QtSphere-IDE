#include "mapdrawcommand.h"

MapDrawCommand::MapDrawCommand(MapView* view, QList<QPoint> points, int tileIndex, QWidget* parent) {
	m_mapView = view;
	m_points = points;
	m_tileIndex = tileIndex;

}

void MapDrawCommand::undo() {

}

void MapDrawCommand::redo() {

}

bool MapDrawCommand::mergeWith(const QUndoCommand* command) {
	return true;
}
