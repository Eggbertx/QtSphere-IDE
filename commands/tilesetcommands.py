from PySide6.QtGui import QUndoCommand

from commands.commandids import CommandIDs
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