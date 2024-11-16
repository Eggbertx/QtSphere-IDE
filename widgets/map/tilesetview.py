from PySide6.QtCore import Qt, Slot, Signal, QPoint
from PySide6.QtGui import QContextMenuEvent, QPixmap
from PySide6.QtWidgets import QWidget, QInputDialog, QMenu

from formats.spheremap import Tileset, Tile
from widgets.wrappedgraphicsview import WrappedGraphicsView

class TilesetView(WrappedGraphicsView):
	tileset: Tileset
	def __init__(self, parent: QWidget|None = None):
		super().__init__(parent)
		self.setContextMenuPolicy(Qt.ContextMenuPolicy.CustomContextMenu)
		self.addAction("Insert Tile", self.onInsertTileSelected)
		self.addAction("Append Tile", self.onAppendTileSelected)
		self.addAction("Delete Tile", self.onDeleteTileSelected)
		self.addAction("").setSeparator(True)
		self.addAction("Insert Tiles", self.onInsertTilesSelected)
		self.addAction("Append Tiles", self.onAppendTilesSelected)
		self.addAction("Delete Tiles", self.onDeleteTilesSelected)
		self.customContextMenuRequested.connect(self.onContextMenuRequested)


	def attachTileset(self, tileset:Tileset):
		for tile in tileset.tiles:
			self.addPixmap(tile.image)
		self.tileset = tileset


	@Slot(QPoint)
	def onContextMenuRequested(self, pos:QPoint):
		self.applyClick(pos)
		pos = self.mapToGlobal(pos)
		menu = QMenu(self)
		menu.addActions(self.actions())
		menu.exec(pos)


	@Slot()
	def onInsertTileSelected(self):
		tile = Tile.fromColor(Qt.GlobalColor.black, self.tileset.tileWidth, self.tileset.tileHeight)
		self.tileset.insertTileAtIndex(self.selectedIndex, tile)
		self.insertPixmapAtSelected(QPixmap.fromImage(tile.image))


	@Slot()
	def onAppendTileSelected(self):
		tile = self.tileset.appendTileFromColor(Qt.GlobalColor.black)
		self.addPixmap(QPixmap.fromImage(tile.image))


	@Slot()
	def onDeleteTileSelected(self):
		pass


	@Slot()
	def onInsertTilesSelected(self):
		selected, accepted = QInputDialog.getInt(self.parentWidget(), "Insert Tiles", "Number of tiles (1-255)", 1, 1, 255)
		if not accepted:
			return
		print(self.selectedIndex)


	@Slot()
	def onAppendTilesSelected(self):
		pass


	@Slot()
	def onDeleteTilesSelected(self):
		pass


