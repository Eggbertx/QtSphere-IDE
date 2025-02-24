from PySide6.QtCore import QSize, Qt, QPoint
from PySide6.QtGui import QUndoCommand, QImage, QPainter

from commands.commandids import CommandIDs
from widgets.map.mapview import MapView
from widgets.map.tilesetview import TilesetView, Tile

class TilesetInsertTilesCommand(QUndoCommand):
	tilesetView: TilesetView
	initialTiles: int
	insertAt: int
	insertCount: int

	@property
	def tileset(self):
		return self.tilesetView.tileset

	def __init__(self, view:TilesetView, insertAt:int, insertCount:int):
		super().__init__()
		self.tilesetView = view
		self.initialTiles = view.numTiles
		self.insertAt = insertAt
		self.insertCount = insertCount


	def undo(self):
		# TODO: put deleted tiles in a list to be inserted by redo() if the list is not empty (as opposed to inserting a black tile)
		self.tilesetView.deleteTiles(self.insertCount, self.insertAt)


	def redo(self):
		self.tilesetView.insertTiles(self.insertCount, self.insertAt)


	def mergeWith(self, other: QUndoCommand) -> bool:
		if other.id() == CommandIDs.InsertTiles.value and \
			isinstance(other, TilesetInsertTilesCommand) and \
			other.initialTiles == self.initialTiles and \
			other.insertAt == self.insertAt and \
			other.insertCount == self.insertCount:
				self.setText("Insert tile(s)")
				return True
		return False


class TilesetAppendTilesCommand(QUndoCommand):
	tilesetView: TilesetView
	initialTiles: int
	appendCount: int

	def __init__(self, view:TilesetView, appendCount:int):
		super().__init__()
		self.tilesetView = view
		self.initialTiles = view.numTiles
		self.appendCount = appendCount


	def undo(self):
		self.tilesetView.deleteTiles(self.appendCount, self.tilesetView.numTiles - self.appendCount)
	

	def redo(self):
		self.tilesetView.appendTiles(self.appendCount)


	def mergeWith(self, other: QUndoCommand) -> bool:
		if other.id() == CommandIDs.AppendTiles.value and \
			isinstance(other, TilesetAppendTilesCommand) and \
			other.initialTiles == self.initialTiles and \
			other.appendCount == self.appendCount:
				self.setText("Append tile(s)")
				return True
		return False


class TilesetRemoveTilesCommand(QUndoCommand):
	tilesetView: TilesetView
	initialTiles: int
	removeAt: int
	removedTiles: list[Tile]
	removeCount:int

	def __init__(self, view: TilesetView, removeAt: int, removeCount: int):
		super().__init__()
		self.tilesetView = view
		self.initialTiles = view.numTiles
		self.removeAt = removeAt
		self.removeCount = removeCount
		self.removedTiles = []


	def undo(self):
		self.tilesetView.importTiles(self.removedTiles, self.removeAt)


	def redo(self):
		self.removedTiles = self.tilesetView.deleteTiles(self.removeCount, self.removeAt)


	def mergeWith(self, other: QUndoCommand) -> bool:
		if other.id() == CommandIDs.RemoveTiles.value and \
			isinstance(other, TilesetRemoveTilesCommand) and \
			other.initialTiles == self.initialTiles and \
			other.removeCount == self.removeCount and \
			len(other.removedTiles) == len(self.removedTiles):
				self.setText("Remove tile(s)")
				return True
		return False


class TileSizeChangedCommand(QUndoCommand):
	mapView: MapView
	tilesetView: TilesetView
	oldSize: QSize
	newSize: QSize
	oldTileImages: list[QImage]
	isScaling: bool
	def __init__(self, mapView:MapView, tilesetView:TilesetView, newSize:QSize, isScaling:bool):
		super().__init__()
		self.mapView = mapView
		self.tilesetView = tilesetView
		self.oldSize = QSize(tilesetView.tileset.tileWidth, tilesetView.tileset.tileHeight)
		self.newSize = newSize
		self.oldTileImages = [tile.image for tile in tilesetView.tileset.tiles]
		self.isScaling = isScaling


	def id(self):
		return CommandIDs.ChangeTileSize.value


	def undo(self):
		for t in range(len(self.tilesetView.tileset.tiles)):
			tile = self.tilesetView.tileset.tiles[t]
			tile.image = self.oldTileImages[t]
		self.tilesetView.tileset.tileWidth = self.oldSize.width()
		self.tilesetView.tileset.tileHeight = self.oldSize.height()
		self.mapView.attachTileset(self.tilesetView.tileset)
		self.tilesetView.attachTileset(self.tilesetView.tileset, True)


	def redo(self):
		self.tilesetView.tileset.tileWidth = self.newSize.width()
		self.tilesetView.tileset.tileHeight = self.newSize.height()
		for t in range(len(self.tilesetView.tileset.tiles)):
			if self.isScaling:
				self.tilesetView.tileset.tiles[t].image = self.oldTileImages[t].scaled(self.newSize, Qt.AspectRatioMode.IgnoreAspectRatio)
			else:
				tmpImg = QImage(self.newSize, QImage.Format.Format_ARGB32)
				tmpImgPainter = QPainter(tmpImg)
				tmpImgPainter.drawImage(QPoint(0, 0), self.oldTileImages[t])
				if self.newSize.width() > self.oldSize.width():
					tmpImgPainter.fillRect(self.oldSize.width(), 0, self.newSize.width() - self.oldSize.width(), self.newSize.height(), Qt.GlobalColor.black)
				if self.newSize.height() > self.oldSize.height():
					tmpImgPainter.fillRect(0, self.oldSize.height(), self.newSize.width(), self.newSize.height() - self.oldSize.height(), Qt.GlobalColor.black)
				tmpImgPainter.end()
				self.tilesetView.tileset.tiles[t].image = tmpImg
		self.mapView.attachTileset(self.tilesetView.tileset)
		self.tilesetView.attachTileset(self.tilesetView.tileset, True)


	def mergeWith(self, other: QUndoCommand) -> bool:
		return other.id() == CommandIDs.ChangeTileSize.value and \
			isinstance(other, TileSizeChangedCommand) and \
			other.tilesetView == self.tilesetView and \
			other.newSize == self.newSize and \
			other.isScaling == self.isScaling
