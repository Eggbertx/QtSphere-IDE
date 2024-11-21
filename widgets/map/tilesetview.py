from PySide6.QtCore import Qt, Slot, Signal, QPoint
from PySide6.QtGui import QContextMenuEvent, QPixmap, QAction
from PySide6.QtWidgets import QWidget, QInputDialog, QMenu

from formats.spheremap import Tileset, Tile
from widgets.wrappedgraphicsview import WrappedGraphicsView

class TilesetView(WrappedGraphicsView):
	tileset: Tileset
	contextMenu: QMenu

	tilesInserted:Signal = Signal(int,int) # args: selectedIndex, count
	tilesAppended:Signal = Signal(int) # args: count
	tilesRemoved:Signal = Signal(int,int) # args: selectedIndex, count

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


	def __insertTile(self):
		tile = Tile.fromColor(Qt.GlobalColor.black, self.tileset.tileWidth, self.tileset.tileHeight)
		self.tileset.insertTileAtIndex(self.selectedIndex, tile)
		self.insertPixmapAtSelected(QPixmap.fromImage(tile.image))


	def __appendTile(self):
		tile = self.tileset.appendTileFromColor(Qt.GlobalColor.black)
		self.addPixmap(QPixmap.fromImage(tile.image))


	def __deleteTile(self):
		if self.numTiles > 1:
			self.tileset.removeTileAtIndex(self.selectedIndex)
			self.removeSelectedPixmap()


	@Slot(QPoint)
	def onContextMenuRequested(self, pos:QPoint):
		self.applyClick(pos)
		self.contextMenu.exec(self.mapToGlobal(pos))


	@Slot()
	def onInsertTileSelected(self):
		self.__insertTile()
		self.tilesInserted.emit(self.selectedIndex, 1)



	@Slot()
	def onAppendTileSelected(self):
		self.__appendTile()
		self.tilesAppended.emit(1)


	@Slot()
	def onDeleteTileSelected(self):
		curSelected = self.selectedIndex
		self.__deleteTile()
		self.tilesRemoved.emit(1, self.selectedIndex)


	@Slot()
	def onInsertTilesSelected(self):
		selected, accepted = QInputDialog.getInt(self.parentWidget(), "Insert Tiles", "Number of tiles (1-255)", 1, 1, 255)
		if accepted:
			for i in range(selected):
				self.__insertTile()
			self.tilesInserted.emit(self.selectedIndex, selected)


	@Slot()
	def onAppendTilesSelected(self):
		selected, accepted = QInputDialog.getInt(self.parentWidget(), "Append Tiles", "Number of tiles (1-255)", 1, 1, 255)
		if accepted:
			for i in range(selected):
				self.__appendTile()
			self.tilesAppended.connect(selected)

	@Slot()
	def onDeleteTilesSelected(self):
		maxDeletable = len(self.tileset.tiles) - self.selectedIndex - 1
		if maxDeletable < 1:
			maxDeletable = 1
		selected, accepted = QInputDialog.getInt(self.parentWidget(), "Append Tiles", f"Number of tiles (1-{maxDeletable})", 1, 1, maxDeletable)
		if accepted:
			numDeleted = 0
			currentIndex = self.selectedIndex
			for i in range(selected):
				if self.numTiles > 1:
					self.__deleteTile()
					numDeleted += 1
			if numDeleted > 0:
				self.tilesRemoved.emit(currentIndex, numDeleted)


	@Slot(bool)
	def onGridSelected(self, isChecked:bool):
		print("Enabling tileset grid:", isChecked)


	@Slot(bool)
	def onObstructionsSelected(self, isChecked:bool):
		print("Enabling tileset obstructions:", isChecked)


	@Slot(int)
	def onTilesetZoomChanged(self, newZoom:int):
		for t in range(self.numTiles):
			self.pixmaps[t] = QPixmap.fromImage(
				self.tileset.tiles[t].image).scaledToHeight(self.tileset.tileHeight*newZoom)
		self.arrangeItems()


	@Slot()
	def onPropertiesSelected(self):
		print("Properties")
