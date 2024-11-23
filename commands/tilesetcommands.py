from PySide6.QtGui import QUndoCommand

from commands.commandids import CommandIDs
from widgets.map.tilesetview import TilesetView

class TilesetInsertTilesCommand(QUndoCommand):
	tilesetView: TilesetView
	initialTiles: int
	insertAt: int
	insertCount: int

	@property
	def tileset(self):
		return self.tilesetView.tileset

	def __init__(self, view:TilesetView, initialTiles:int, insertAt:int, insertCount:int):
		super().__init__(None)
		self.tilesetView = view
		self.initialTiles = initialTiles
		self.insertAt = insertAt
		self.insertCount = insertCount


	def undo(self):
		# TODO: put deleted tiles in a list to be inserted by redo() if the list is not empty (as opposed to inserting a black tile)
		self.tilesetView.deleteTiles(self.insertCount, self.insertAt)


	def redo(self):
		self.tilesetView.insertTiles(self.insertCount, self.insertAt)


	def mergeWith(self, other: QUndoCommand) -> bool:
		if other.id() == CommandIDs.InsertTiles.value and isinstance(other, TilesetInsertTilesCommand) and other.initialTiles == self.initialTiles and other.insertAt == self.insertAt and other.insertCount == self.insertCount:
			self.setText("Insert tile(s)")
			return True
		return False


class TilesetAppendTilesCommand(QUndoCommand):
	tilesetView: TilesetView
	initialTiles: int
	appendCount: int

	def __init__(self, view:TilesetView, initialTiles:int, appendCount:int):
		super().__init__()
		self.tilesetView = view
		self.initialTiles = initialTiles
		self.appendCount = appendCount


	def undo(self):
		self.tilesetView.deleteTiles(self.appendCount, self.tilesetView.numTiles - self.appendCount)
	

	def redo(self):
		self.tilesetView.appendTiles(self.appendCount)


	def mergeWith(self, other: QUndoCommand) -> bool:
		if other.id() == CommandIDs.AppendTiles.value and isinstance(other, TilesetAppendTilesCommand) and other.initialTiles == self.initialTiles and other.appendCount == self.appendCount:
			self.setText("Append tile(s)")
			return True
		return False