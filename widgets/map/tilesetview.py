from PySide6.QtCore import Qt, Slot, Signal, QPoint
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import QWidget, QInputDialog, QMenu

from formats.spheremap import Tileset, Tile
from widgets.wrappedgraphicsview import WrappedGraphicsView

class TilesetView(WrappedGraphicsView):
	tileset: Tileset
	contextMenu: QMenu
	mapEditor: QWidget

	tilesInserted:Signal = Signal(int,int) # args: selectedIndex, count
	tilesAppended:Signal = Signal(int) # args: count
	tilesRemoved:Signal = Signal(int,list) # args: selectedIndex, count

	@property
	def numTiles(self):
		if self.tileset is None:
			return 0
		return len(self.tileset.tiles)


	def __init__(self, parent: QWidget|None = None):
		super().__init__(parent)
		self.setContextMenuPolicy(Qt.ContextMenuPolicy.CustomContextMenu)
		self.contextMenu = QMenu(self)
		self.contextMenu.addAction("Insert Tile", self.onInsertTileSelected)
		self.contextMenu.addAction("Append Tile", self.onAppendTileSelected)
		self.contextMenu.addAction("Delete Tile", self.onDeleteTileSelected)
		self.contextMenu.addSeparator()
		self.contextMenu.addAction("Insert Tiles", self.onInsertTilesSelected)
		self.contextMenu.addAction("Append Tiles", self.onAppendTilesSelected)
		self.contextMenu.addAction("Delete Tiles", self.onDeleteTilesSelected)
		self.contextMenu.addSeparator()

		viewMenu = self.contextMenu.addMenu("View")
		toggleGridAction = viewMenu.addAction("Grid")
		toggleGridAction.triggered.connect(lambda: self.onGridSelected(toggleGridAction.isChecked()))
		toggleGridAction.setCheckable(True)
		toggleTilesetObstructionsAction = viewMenu.addAction("Obstructions")
		toggleTilesetObstructionsAction.triggered.connect(lambda: self.onObstructionsSelected(toggleTilesetObstructionsAction.isChecked()))
		toggleTilesetObstructionsAction.setCheckable(True)

		zoomMenu = self.contextMenu.addMenu("Zoom")
		zoomMenu.addAction(f"1x", lambda: self.onTilesetZoomChanged(1))
		zoomMenu.addAction(f"2x", lambda: self.onTilesetZoomChanged(2))
		zoomMenu.addAction(f"4x", lambda: self.onTilesetZoomChanged(4))
		zoomMenu.addAction(f"8x", lambda: self.onTilesetZoomChanged(8))

		self.contextMenu.addSeparator()
		self.contextMenu.addAction("Properties", self.onPropertiesSelected)

		self.customContextMenuRequested.connect(self.onContextMenuRequested)


	def attachTileset(self, tileset:Tileset):
		for tile in tileset.tiles:
			self.addPixmap(tile.image)
		self.tileset = tileset


	def insertTile(self):
		self.insertTiles(1)


	def insertTiles(self, count:int, index = -1):
		if index == -1:
			index = self.selectedIndex
		for i in range(count):
			tile = Tile.fromColor(Qt.GlobalColor.black, self.tileset.tileWidth, self.tileset.tileHeight)
			self.tileset.insertTileAtIndex(index, tile)
			self.insertPixmapAtSelected(QPixmap.fromImage(tile.image))


	def appendTile(self):
		self.appendTiles(1)


	def appendTiles(self, count:int):
		for i in range(count):
			tile = self.tileset.appendTileFromColor(Qt.GlobalColor.black)
			self.addPixmap(QPixmap.fromImage(tile.image))


	def deleteTile(self):
		return self.deleteTiles(1)


	def deleteTiles(self, deleting:int, index = -1):
		if index == -1:
			index = self.selectedIndex
		deleted:list[Tile] = []
		for i in range(deleting):
			if self.numTiles > 1:
				tile = self.tileset.removeTileAtIndex(index)
				if tile is not None:
					self.removePixmap(index)
					deleted.append(tile)
		return deleted


	@Slot(QPoint)
	def onContextMenuRequested(self, pos:QPoint):
		self.applyClick(pos)
		self.contextMenu.exec(self.mapToGlobal(pos))


	@Slot()
	def onInsertTileSelected(self):
		# self.insertTile()
		self.tilesInserted.emit(self.selectedIndex, 1)
		# if self.mapEditor is not None:
		# 	self.mapEditor.undoStack.push(TilesetInsertTileCommand(self, self.numTiles, self.selectedIndex, 1))
		# # self.undoStack.push(DirectionAppendFrameCommand(self, self.directionIndex))


	@Slot()
	def onAppendTileSelected(self):
		self.appendTile()


	@Slot()
	def onDeleteTileSelected(self):
		self.deleteTile()


	@Slot()
	def onInsertTilesSelected(self):
		selected, accepted = QInputDialog.getInt(self.parentWidget(), "Insert Tiles", "Number of tiles (1-255)", 1, 1, 255)
		if accepted:
			self.tilesInserted.emit(self.selectedIndex, selected)
			# for i in range(selected):
			# 	self.insertTile()


	@Slot()
	def onAppendTilesSelected(self):
		selected, accepted = QInputDialog.getInt(self.parentWidget(), "Append Tiles", "Number of tiles (1-255)", 1, 1, 255)
		if accepted:
			for i in range(selected):
				self.appendTile()


	@Slot()
	def onDeleteTilesSelected(self):
		maxDeletable = len(self.tileset.tiles) - self.selectedIndex - 1
		if maxDeletable < 1:
			maxDeletable = 1
		selected, accepted = QInputDialog.getInt(self.parentWidget(), "Append Tiles", f"Number of tiles (1-{maxDeletable})", 1, 1, maxDeletable)
		if accepted and selected > 0:
			self.deleteTiles(selected)


	@Slot(bool)
	def onGridSelected(self, isChecked:bool):
		print("Enabling tileset grid:", isChecked)


	@Slot(bool)
	def onObstructionsSelected(self, isChecked:bool):
		print("Enabling tileset obstructions:", isChecked)


	@Slot(int)
	def onTilesetZoomChanged(self, newZoom:int):
		self.setZoom(newZoom)


	@Slot()
	def onPropertiesSelected(self):
		print("Properties")
