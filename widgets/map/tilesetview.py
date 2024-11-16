from PySide6.QtCore import Qt, Slot, Signal, QPoint
from PySide6.QtGui import QContextMenuEvent, QPixmap, QAction
from PySide6.QtWidgets import QWidget, QInputDialog, QMenu

from formats.spheremap import Tileset, Tile
from widgets.wrappedgraphicsview import WrappedGraphicsView

class TilesetView(WrappedGraphicsView):
	tileset: Tileset
	contextMenu: QMenu

	tileInserted:Signal = Signal(int)
	tileRemoved:Signal = Signal(int)
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
		for z in range(4):
			zoomMenu.addAction(f"{z+1}x", lambda: self.onTilesetZoomChanged(z+1))

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
		self.tileInserted.emit(self.selectedIndex)


	def __appendTile(self):
		tile = self.tileset.appendTileFromColor(Qt.GlobalColor.black)
		self.addPixmap(QPixmap.fromImage(tile.image))


	def __deleteTile(self):
		if len(self.tileset) > 1:
			curSelected = self.selectedIndex
			self.tileset.removeTileAtIndex(self.selectedIndex)
			self.removeSelectedPixmap()
			self.tileRemoved.emit(curSelected)


	@Slot(QPoint)
	def onContextMenuRequested(self, pos:QPoint):
		self.applyClick(pos)
		self.contextMenu.exec(self.mapToGlobal(pos))


	@Slot()
	def onInsertTileSelected(self):
		self.__insertTile()


	@Slot()
	def onAppendTileSelected(self):
		self.__appendTile()


	@Slot()
	def onDeleteTileSelected(self):
		self.__deleteTile()


	@Slot()
	def onInsertTilesSelected(self):
		selected, accepted = QInputDialog.getInt(self.parentWidget(), "Insert Tiles", "Number of tiles (1-255)", 1, 1, 255)
		if accepted:
			for i in range(selected):
				self.__insertTile()


	@Slot()
	def onAppendTilesSelected(self):
		selected, accepted = QInputDialog.getInt(self.parentWidget(), "Append Tiles", "Number of tiles (1-255)", 1, 1, 255)
		if accepted:
			for i in range(selected):
				self.__appendTile()


	@Slot()
	def onDeleteTilesSelected(self):
		maxDeletable = len(self.tileset.tiles) - self.selectedIndex - 1
		if maxDeletable < 1:
			maxDeletable = 1
		selected, accepted = QInputDialog.getInt(self.parentWidget(), "Append Tiles", f"Number of tiles (1-{maxDeletable})", 1, 1, maxDeletable)
		if accepted:
			for i in range(selected):
				self.__deleteTile()


	@Slot(bool)
	def onGridSelected(self, isChecked:bool):
		print("Enabling tileset grid:", isChecked)


	@Slot(bool)
	def onObstructionsSelected(self, isChecked:bool):
		print("Enabling tileset obstructions:", isChecked)


	@Slot(int)
	def onTilesetZoomChanged(self, newZoom:int):
		print("Tileset zoom:", newZoom)


	@Slot()
	def onPropertiesSelected(self):
		print("Properties")
