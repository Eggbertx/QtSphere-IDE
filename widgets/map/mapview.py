from enum import Enum, auto
from math import floor

from PySide6.QtCore import Qt, QEvent, QPoint, QRect, Signal, Slot, QSize
from PySide6.QtGui import QMouseEvent, QPixmap, QColor
from PySide6.QtWidgets import QGraphicsItemGroup, QGraphicsScene, QGraphicsView, QWidget, QGraphicsPixmapItem, QGraphicsLineItem, QGraphicsRectItem, QGraphicsTextItem


from formats.spheremap import SphereMap, EntityType
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
	pointerGroup: QGraphicsItemGroup
	drawing: bool
	currentTile: int
	currentLayer: int
	hoverTilePos: QPoint
	hoverTilePosChanged: Signal = Signal(QPoint)

	#map icons
	spIcon:QGraphicsPixmapItem
	personIcons:list[QGraphicsPixmapItem]
	triggerIcons:list[QGraphicsPixmapItem]

	@property
	def spawnPointIconVisible(self):
		return self.spIcon.isVisible()
	
	@spawnPointIconVisible.setter
	def spawnPointIconVisible(self, v:bool):
		self.spIcon.setVisible(v)

	@property
	def personIconsVisible(self):
		if len(self.personIcons) == 0:
			return False
		return self.personIcons[0].isVisible()

	@personIconsVisible.setter
	def personIconsVisible(self, v:bool):
		for p in range(len(self.personIcons)):
			self.personIcons[p].setVisible(v)
	
	@property
	def triggerIconsVisible(self):
		if len(self.triggerIcons) == 0:
			return False
		return self.triggerIcons[0].isVisible()

	@triggerIconsVisible.setter
	def triggerIconsVisible(self, v:bool):
		for t in range(len(self.triggerIcons)):
			self.triggerIcons[t].setVisible(v)

	@property
	def gridVisible(self):
		return self.gridGroup.isVisible()

	@gridVisible.setter
	def gridVisible(self, visible:bool):
		self.gridGroup.setVisible(visible)


	def __init__(self, parent: QWidget = None):
		super().__init__(parent)
		self.setMouseTracking(True)
		self.pointerGroup = None
		self.gridGroup = QGraphicsItemGroup()
		self.gridVisible = False
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

		self.personIcons = []
		self.triggerIcons = []


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
		self.__updateMapIcons(tileW, tileH)
		self.gridVisible = False


	def __updateMapIcons(self, tileW:int, tileH:int):
		spawnPointPixmap = QPixmap(":/res/spawnpoint_icon.svg")
		if spawnPointPixmap.height() > tileH:
			spawnPointPixmap = spawnPointPixmap.scaledToHeight(tileH, Qt.TransformationMode.SmoothTransformation)
		if spawnPointPixmap.width() > tileW:
			spawnPointPixmap = spawnPointPixmap.scaledToWidth(tileW, Qt.TransformationMode.SmoothTransformation)

		self.spIcon = self.mapScene.addPixmap(spawnPointPixmap)
		startX = self.mapFile.startX - tileW/2+1 if self.mapFile.startX > 0 else 0
		startY = self.mapFile.startY - tileH/2+1 if self.mapFile.startY > 0 else 0
		self.spIcon.setPos(startX, startY)

		personIcon = QPixmap(":/res/person.svg")
		if personIcon.height() > tileH:
			personIcon = personIcon.scaledToHeight(tileH, Qt.TransformationMode.SmoothTransformation)
		if personIcon.width() > tileW:
			personIcon = personIcon.scaledToWidth(tileW, Qt.TransformationMode.SmoothTransformation)
		
		triggerIcon = QPixmap(":/res/trigger.svg")
		if triggerIcon.height() > tileH:
			triggerIcon = triggerIcon.scaledToHeight(tileH, Qt.TransformationMode.SmoothTransformation)
		if triggerIcon.width() > tileW:
			triggerIcon = triggerIcon.scaledToWidth(tileW, Qt.TransformationMode.SmoothTransformation)

		for entity in self.mapFile.entities:
			entityX = entity.mapX - tileW/2+1 if entity.mapX > 0 else 0
			entityY = entity.mapY - tileH/2+1 if entity.mapY > 0 else 0
			item = self.mapScene.addPixmap(personIcon if entity.type == EntityType.Person else triggerIcon)
			item.setPos(entityX, entityY)
			item.setZValue(256)
			if entity.type == EntityType.Person:
				self.personIcons.append(item)
			else:
				self.triggerIcons.append(item)


	def setLayerVisible(self, layer:int, visible:bool):
		if layer < 0 or layer >= len(self.mapFile.layers):
			raise IndexError(f"Layer index {layer} does not exist")
		self.mapFile.layers[layer].visible = visible
		items = self.mapScene.items()
		for item in items:
			if item.zValue() == layer and item.type != 10:
				item.setVisible(visible)


	def toggleLayerVisibility(self, layer:int):
		self.setLayerVisible(layer, not self.mapFile.layers[layer].visible)
		return self.mapFile.layers[layer].visible


	def deleteLayer(self, layer:int):
		if layer < 0 or layer >= len(self.mapFile.layers):
			raise IndexError(f"Layer index {layer} does not exist")
		items = self.mapScene.items()
		for item in items:
			if item.zValue() == layer and item.type != 10:
				self.mapScene.removeItem(item)


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

		self.pointerGroup.setZValue(258)


	def setDrawSize(self, size:int):
		self.drawSize = size
		self.__resetPointerGroup()
		self.__updatePointer()


	def widgetToMapPos(self, x:int, y:int):
		if self.mapFile is None:
			return QPoint(-1, -1)
		return QPoint(floor(x/self.mapFile.tileset.tileWidth), floor(y/self.mapFile.tileset.tileHeight))


	def pointerRect(self, tiles:bool):
		rX = self.pointerGroup.x()
		rY = self.pointerGroup.y()
		if tiles:
			tilePos = self.widgetToMapPos(
				rX + self.horizontalScrollBar().value(),
				rY + self.verticalScrollBar().value()
			)
			rX = tilePos.x()
			rY = tilePos.y()

		rSize = QSize(self.drawSize, self.drawSize)
		if not tiles:
			rSize.setWidth(rSize.width() * self.mapFile.tileset.tileWidth)
			rSize.setHeight(rSize.height() * self.mapFile.tileset.tileWidth)
		return QRect(QPoint(rX, rY), rSize)

	def mouseMoveEvent(self, event: QMouseEvent):
		if self.mapFile is None:
			self.setStatusTip("Map not loaded")
			return

		mapSize = self.mapFile.largestLayerSize()
		tileW = self.mapFile.tileset.tileWidth
		tileH = self.mapFile.tileset.tileHeight
		mapRect = QRect(0, 0, mapSize.width() * tileW, mapSize.height() * tileH)
		
		tilePos = self.widgetToMapPos(
			event.pos().x() + self.horizontalScrollBar().value() - 1,
			event.pos().y() + self.verticalScrollBar().value() - 1
		)
		if tilePos != self.hoverTilePos:
			self.hoverTilePos = tilePos
			self.hoverTilePosChanged.emit(self.hoverTilePos)

		if mapRect.contains(event.pos()):
			self.window().setStatus("Map tile: ({},{}) Pixel: ({},{})".format(
				tilePos.x(), tilePos.y(), event.pos().x(), event.pos().y()
			))
		else:
			self.hoverTilePos = QPoint(-1,-1)
			self.hoverTilePosChanged.emit(self.hoverTilePos)
			self.window().setStatus("")
		self.setSceneRect(mapRect)


	def mousePressEvent(self, event: QMouseEvent) -> None:
		if event.button() == Qt.MouseButton.LeftButton:
			self.drawing = True
			self.__drawTile()


	def mouseReleaseEvent(self, event: QMouseEvent) -> None:
		self.drawing = False


	def leaveEvent(self, event: QEvent):
		super().leaveEvent(event)
		self.pointerGroup.hide()
		self.drawing = False
		self.window().setStatus("")


	def __drawTile(self):
		tileWidth = self.mapFile.tileset.tileWidth
		tileHeight = self.mapFile.tileset.tileHeight
		pRect = self.pointerRect(True)
		print(self.currentTile, self.currentLayer)
		for rY in range(pRect.y(), pRect.bottom() + 1, 1):
			for rX in range(pRect.x(), pRect.right() + 1, 1):
				sceneX = rX * tileWidth
				sceneY = rY * tileHeight
				mouseItems = self.items(sceneX, sceneY)
				for item in mouseItems:
					if isinstance(item, QGraphicsPixmapItem) and item.zValue() == len(self.mapFile.layers)-self.currentLayer-1:
						item.setPixmap(QPixmap.fromImage(self.mapFile.tileset.tiles[self.currentTile].image))
						break


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

		self.mapScene.addItem(self.gridGroup)
		self.gridGroup.setZValue(257)


	@Slot(int)
	def onTileIndexChanged(self, newIndex:int):
		self.currentTile = newIndex

	@Slot(QPoint)
	def onHoverTilePosChanged(self, pos:QPoint):
		pointerUL = self.mapToWidgetPos(
			pos.x() - floor(self.drawSize/2),
			pos.y() - floor(self.drawSize/2)
		)
		self.pointerGroup.setPos(pointerUL)
		if pos.x() > -1 and pos.y() > -1:
			self.pointerGroup.show()
			if self.drawing:
				self.__drawTile()
		else:
			self.pointerGroup.hide()