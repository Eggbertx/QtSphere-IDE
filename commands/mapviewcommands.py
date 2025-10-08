from PySide6.QtCore import QPoint
from PySide6.QtGui import QUndoCommand

from commands.commandids import CommandIDs
from widgets.map.mapview import MapView

class PencilDrawMapCommand(QUndoCommand):
	affectedTiles: list[QPoint] # stores the points (in tile coordinates) affected by the pencil drawing operation
	oldTiles:list[int] # stores the old tile index before the pencil drawing operation, to be restored when undoing
	newIndex:int
	layer:int
	mapView: MapView
	def __init__(self, affectedTiles:list[QPoint], newIndex:int, layer:int, mapView:MapView):
		super().__init__()
		self.oldTiles = []
		self.affectedTiles = affectedTiles
		self.newIndex = newIndex
		self.layer = layer
		self.mapView = mapView


	def id(self) -> int:
		return CommandIDs.PencilDrawMap.value


	def undo(self):
		for i, tilePos in enumerate(self.affectedTiles):
			mapX = tilePos.x() * self.mapView.tileWidth
			mapY = tilePos.y() * self.mapView.tileHeight
			tileIndex = self.mapView.getTileIndexAt(mapX, mapY, self.layer)
			if tileIndex == -1 or tileIndex >= len(self.mapView.mapFile.tileset.tiles):
				continue
			self.mapView.setTileIndexAt(mapX, mapY, self.layer, self.oldTiles[i])
		self.mapView.removeTemporaryTiles()


	def redo(self):
		for tilePos in self.affectedTiles:
			mapX = tilePos.x() * self.mapView.tileWidth
			mapY = tilePos.y() * self.mapView.tileHeight
			tileIndex = self.mapView.getTileIndexAt(mapX, mapY, self.layer)
			if tileIndex == -1 or tileIndex >= len(self.mapView.mapFile.tileset.tiles):
				continue
			self.oldTiles.append(tileIndex)
			self.mapView.setTileIndexAt(mapX, mapY, self.layer, self.newIndex)
		self.mapView.removeTemporaryTiles()


	def mergeWith(self, other):
		if self.id() == other.id() and \
			isinstance(other, PencilDrawMapCommand) and \
			self.affectedTiles == other.affectedTiles and \
			self.newIndex == other.newIndex and \
			self.layer == other.layer:
			self.setText("Pencil draw")
			return True
		return False