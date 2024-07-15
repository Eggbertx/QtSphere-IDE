from enum import Enum, auto
from math import floor
from PySide6.QtCore import QEvent, QPoint, QRect, Signal, Slot
from PySide6.QtGui import QMouseEvent, QPixmap, QColor
from PySide6.QtWidgets import QGraphicsItemGroup, QGraphicsScene, QGraphicsView, QWidget, QGraphicsPixmapItem, QGraphicsLineItem, QGraphicsRectItem

from formats.spheremap import SphereMap
from settings import Settings, Defaults

class MapTool(Enum):
	Pencil = auto()
	Line = auto()
	Rectangle = auto()
	Fill = auto()
	Select = auto()

class MapView(QGraphicsView):
	mapScene: QGraphicsScene
	mapFile: SphereMap
	drawSize:int
	gridGroup: QGraphicsItemGroup
	__gridVisible: bool
	pointerGroup: QGraphicsItemGroup
	drawing: bool
	currentTile: int
	currentLayer: int
	hoverTilePos: QPoint
	hoverTilePosChanged: Signal = Signal(QPoint)

	@property
	def gridVisible(self):
		return self.__gridVisible

	@gridVisible.setter
	def gridVisible(self, visible:bool):
		self.__gridVisible = visible
		self.gridGroup.setVisible(visible)


	def __init__(self, parent: QWidget = None):
		super().__init__(parent)
		self.setMouseTracking(True)
		self.pointerGroup = None
		self.gridGroup = QGraphicsItemGroup()
		self.__gridVisible = False
		self.gridGroup.setVisible(False)
		self.mapScene = QGraphicsScene(self)
		self.setScene(self.mapScene)

		self.mapFile = None
		self.drawSize = 3
		self.drawing = False
		self.currentTile = 0
		self.currentLayer = 0

		self.setMouseTracking(True)
		self.hoverTilePos = QPoint(-1, -1)
		self.hoverTilePosChanged.connect(self.onHoverTilePosChanged)
		self.setViewportUpdateMode(QGraphicsView.ViewportUpdateMode.FullViewportUpdate)


	def attachMap(self, map:SphereMap):
		tileW = map.tileset.tileWidth
		tileH = map.tileset.tileHeight
		self.mapScene.clear()
		for l in range(len(map.layers)):
			layer = map.layers[l]
			for t in range(len(layer.tiles)):
				tile = layer.tiles[t]
				tilePixmap = self.mapScene.addPixmap(QPixmap.fromImage(map.tileset.tiles[tile].image))
				tilePixmap.setShapeMode(QGraphicsPixmapItem.ShapeMode.BoundingRectShape)
				x = t % layer.width
				y = (t - x) / layer.width
				tilePixmap.setPos(x * tileW,y * tileH)
				tilePixmap.setZValue(l)
				tilePixmap.setVisible(layer.visible)
		self.mapFile = map
		self.setDrawSize(1)
		self.__updateGrid()


	def mapToWidgetPos(self, x:int, y:int):
		if self.mapFile is None:
			return QPoint(-1, -1)
		return QPoint(x * self.mapFile.tileset.tileWidth, y * self.mapFile.tileset.tileHeight)


	def setCurrentTool(self, tool:MapTool):
		pass


	def __updatePointer(self):
		settings = Settings()
		tw = self.mapFile.tileset.tileWidth
		th = self.mapFile.tileset.tileHeight
		cursorColor = QColor(settings.mapCursorColor)
		cursorColor.setAlpha(128)

		for y in range(self.drawSize):
			for x in range(self.drawSize):
				item = QGraphicsRectItem(x * tw, y * th, tw, th)
				item.setBrush(cursorColor)
				self.pointerGroup.addToGroup(item)

		self.pointerGroup.setZValue(257)


	def setDrawSize(self, size:int):
		self.drawSize = size
		self.__resetPointerGroup()
		self.__updatePointer()


	def widgetToMapPos(self, x:int, y:int):
		if self.mapFile is None:
			return QPoint(-1, -1)
		return QPoint(floor(x/self.mapFile.tileset.tileWidth), floor(y/self.mapFile.tileset.tileHeight))


	def mouseMoveEvent(self, event: QMouseEvent):
		if self.mapFile is None:
			self.setStatusTip("Map not loaded")
			return

		mapSize = self.mapFile.largestLayerSize()
		tileW = self.mapFile.tileset.tileWidth
		tileH = self.mapFile.tileset.tileHeight
		widgetRect = QRect(0, 0, mapSize.width() * tileW, mapSize.height() * tileH)
		
		tilePos = self.widgetToMapPos(
			event.pos().x() + self.horizontalScrollBar().value() - 1,
			event.pos().y() + self.verticalScrollBar().value() - 1
		)
		if tilePos != self.hoverTilePos:
			self.hoverTilePos = tilePos
			self.hoverTilePosChanged.emit(self.hoverTilePos)

		if widgetRect.contains(event.pos()):
			self.window().setStatus("Map tile: ({},{}) Pixel: ({},{})".format(
				tilePos.x(), tilePos.y(), event.pos().x(), event.pos().y()
			))
		else:
			self.hoverTilePos = QPoint(-1,-1)
			self.hoverTilePosChanged.emit(self.hoverTilePos)
			self.window().setStatus("")
		self.setSceneRect(widgetRect)


	def leaveEvent(self, event: QEvent):
		super().leaveEvent(event)
		self.pointerGroup.hide()
		self.drawing = False
		self.window().setStatus("")


	def __resetPointerGroup(self):
		if self.pointerGroup is not None:
			items = self.pointerGroup.childItems()
			for item in items:
				self.mapScene.removeItem(item)
			self.mapScene.removeItem(self.pointerGroup)

		self.pointerGroup = QGraphicsItemGroup()
		self.mapScene.addItem(self.pointerGroup)


	def __resetGridGroup(self):
		if self.gridGroup is not None:
			if self.gridGroup.scene() is not None:
				self.mapScene.destroyItemGroup(self.gridGroup)
			del self.gridGroup
		self.gridGroup = QGraphicsItemGroup()


	def __updateGrid(self):
		self.__resetGridGroup()
		settings = Settings()
		gridColor = settings.gridColor
		if not gridColor.isValid():
			gridColor = Defaults.gridColor.value
			settings.gridColor = gridColor
		gridColor.setAlpha(128)

		mapSize = self.sceneRect().size()
		mapWidth = int(mapSize.width())
		mapHeight = int(mapSize.height())
		
		sceneWidth = self.mapScene.width()
		sceneHeight = self.mapScene.height()

		gridWidth = self.mapFile.tileset.tileWidth
		gridHeight = self.mapFile.tileset.tileHeight

		for y in range(gridHeight, mapHeight, gridHeight):
			line = QGraphicsLineItem(0, y, sceneWidth-1, y)
			line.setPen(gridColor)
			self.gridGroup.addToGroup(line)
		
		for x in range(gridWidth, mapWidth, gridWidth):
			line = QGraphicsLineItem(x, 0, x, sceneHeight-1)
			line.setPen(gridColor)
			self.gridGroup.addToGroup(line)

		self.gridGroup.setVisible(self.__gridVisible)
		self.mapScene.addItem(self.gridGroup)
		self.gridGroup.setZValue(256)


	@Slot(QPoint)
	def onHoverTilePosChanged(self, pos:QPoint):
		pointerUL = self.mapToWidgetPos(
			pos.x() - floor(self.drawSize/2),
			pos.y() - floor(self.drawSize/2)
		)
		self.pointerGroup.setPos(pointerUL)
		if pos.x() > -1 and pos.y() > -1:
			self.pointerGroup.show()
		else:
			self.pointerGroup.hide()