from enum import Enum, auto

import shiboken6
from PySide6.QtCore import Qt, QEvent, QPoint, QRect, Signal, Slot, QSize
from PySide6.QtGui import QMouseEvent, QPixmap, QColor, QAction
from PySide6.QtWidgets import (
	QGraphicsItemGroup, QGraphicsScene, QGraphicsView, QWidget, QGraphicsPixmapItem, QGraphicsItem, QGraphicsLineItem, QGraphicsRectItem, QMenu
)

from formats.spheremap import SphereMap, EntityType
from formats.tileset import Tileset
from settings import Settings, Defaults

from widgets.drawingenums import DrawingTool, DrawMode


class MapView(QGraphicsView):
	mapScene: QGraphicsScene
	mapFile: SphereMap
	drawSize:int
	gridGroup: QGraphicsItemGroup
	pointerGroup: QGraphicsItemGroup
	drawing: bool
	drawMode: DrawMode
	drawingPoints: list[QPoint]
	currentTile: int
	currentLayer: int
	hoverTilePos: QPoint
	hoverTilePosChanged: Signal = Signal(QPoint)

	# Map icons
	spIcon: QGraphicsPixmapItem
	personIcons: list[QGraphicsPixmapItem]
	triggerIcons: list[QGraphicsPixmapItem]


	# Context menu
	contextMenu: QMenu
	selectTileAction: QAction
	setEntryPointAction: QAction
	newPersonAction: QAction
	newTriggerAction: QAction
	editEntityAction: QAction
	deleteEntityAction: QAction
	editZoneAction: QAction
	
	currentTileChanged:Signal = Signal(int)
	setEntryPointRequested:Signal = Signal(QPoint,int)
	newPersonRequested:Signal = Signal(QPoint,int)
	newTriggerRequested:Signal = Signal(QPoint,int)
	editEntityRequested:Signal = Signal(QPoint,int)
	deleteEntityRequested:Signal = Signal(QPoint,int)
	editZoneRequested:Signal = Signal(QPoint,int)
	drawModeChanged:Signal = Signal(DrawMode)


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


	@property
	def tileWidth(self):
		if self.mapFile is None:
			return 0
		return self.mapFile.tileset.tileWidth


	@property
	def tileHeight(self):
		if self.mapFile is None:
			return 0
		return self.mapFile.tileset.tileHeight


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
		self.drawMode = DrawMode.NotDrawing
		self.drawingPoints = []
		self.currentTile = 0
		self.currentLayer = 0

		self.setMouseTracking(True)
		self.hoverTilePos = QPoint(-1, -1)
		self.hoverTilePosChanged.connect(self.onHoverTilePosChanged)
		self.setViewportUpdateMode(QGraphicsView.ViewportUpdateMode.FullViewportUpdate)

		self.personIcons = []
		self.triggerIcons = []
		self.setContextMenuPolicy(Qt.ContextMenuPolicy.CustomContextMenu)

		self.customContextMenuRequested.connect(self.onCustomContextMenuRequested)
		self.setupContextMenu()


	def setupContextMenu(self):
		self.contextMenu = QMenu()
		self.selectTileAction = self.contextMenu.addAction("Select Tile")
		self.contextMenu.addSeparator()
		self.setEntryPointAction = self.contextMenu.addAction("Set Entry Point")
		entityMenu = self.contextMenu.addMenu("New Entity")
		self.newPersonAction = entityMenu.addAction("Person")
		self.newTriggerAction = entityMenu.addAction("Trigger")
		self.editEntityAction = self.contextMenu.addAction("Edit Entity")
		self.deleteEntityAction = self.contextMenu.addAction("Delete Entity")
		self.editZoneAction = self.contextMenu.addAction("Edit Zone")


	def attachMap(self, map:SphereMap):
		self.mapFile = map
		self.attachTileset(map.tileset)
		self.setDrawSize(1)
		self.gridVisible = False


	def attachTileset(self, tileset:Tileset):
		self.mapScene.clear()
		for l in range(len(self.mapFile.layers)):
			layer = self.mapFile.layers[l]
			for t in range(len(layer.tiles)):
				tile = layer.tiles[t]
				tilePixmap = self.mapScene.addPixmap(QPixmap.fromImage(tileset.tiles[tile].image))
				tilePixmap.setData(0, tileset.tiles[tile].uuid)
				tilePixmap.setShapeMode(QGraphicsPixmapItem.ShapeMode.BoundingRectShape)
				x = t % layer.width
				y = (t - x) / layer.width
				tilePixmap.setPos(x * tileset.tileWidth, y * tileset.tileHeight)
				tilePixmap.setZValue(l)
				tilePixmap.setVisible(layer.visible)
		largestLayer = self.mapFile.largestLayerSize()
		self.setSceneRect(0, 0, largestLayer.width() * tileset.tileWidth, largestLayer.height() * tileset.tileHeight)
		self.__updateGrid()
		self.__updateMapIcons(tileset.tileWidth, tileset.tileHeight)
		self.pointerGroup = None
		self.__resetPointerGroup()
		self.__updatePointer()


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


	def isLayerVisible(self, layer:int) -> bool:
		if layer < 0 or layer >= len(self.mapFile.layers):
			raise IndexError(f"Layer index {layer} does not exist")
		return self.mapFile.layers[layer].visible


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
		return QPoint(x * self.tileWidth, y * self.tileHeight)


	def setCurrentTool(self, tool:DrawingTool):
		pass


	def __updatePointer(self):
		settings = Settings()
		tw = self.tileWidth
		th = self.tileHeight
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
		scenePos = self.mapToScene(x, y)

		p = QPoint(
			int(scenePos.x()) // self.tileWidth,
			int(scenePos.y()) // self.tileHeight
		)
		return p


	def pointerRect(self, tiles:bool):
		if self.pointerGroup is None or not shiboken6.isValid(self.pointerGroup):
			self.__resetPointerGroup()
		rPos = self.pointerGroup.scenePos().toPoint()
		rSize = QSize(self.drawSize * self.tileWidth, self.drawSize * self.tileHeight)
		if tiles:
			# return a rect with tile position and draw size
			widgetPos = self.mapFromScene(rPos)
			rPos = self.widgetToMapPos(widgetPos.x(), widgetPos.y())
			rSize = QSize(self.drawSize, self.drawSize)
		return QRect(rPos, rSize)


	# returns the tile matching the pixmap at the pixel position, or -1 if no tile is found
	def getTileIndexAt(self, x:int, y:int, layer:int, tempTile:bool = False) -> int:
		if self.mapFile is None:
			return -1

		items:list[QGraphicsPixmapItem] = list(filter(lambda i:
			isinstance(i, QGraphicsPixmapItem) and i.zValue() == layer + (0.5 if tempTile else 0), self.items(x, y)
		)) # type: ignore
		assert len(items) <= 1, f"More than one item found at {x}, {y}, layer {layer} (tempTile: {tempTile})"

		for item in items:
			for t, tile in enumerate(self.mapFile.tileset.tiles):
				itemUuid = item.data(0)
				tileUuid = tile.uuid
				if itemUuid == tileUuid:
					return t
		return -1


	def setTileIndexAt(self, x:int, y:int, layer:int, index:int):
		if self.mapFile is None or index < 0 or index >= len(self.mapFile.tileset.tiles):
			return
		widgetPos = self.mapFromScene(self.mapToScene(x, y))
		items:list[QGraphicsPixmapItem] = list(filter(
			lambda i: isinstance(i, QGraphicsPixmapItem) and i.zValue() == layer,
			self.items(widgetPos.x(), widgetPos.y())
		))
		for item in items:
			item.setPixmap(QPixmap.fromImage(self.mapFile.tileset.tiles[index].image))
			self.mapFile.setTileIndexAt(x // self.tileWidth, y // self.tileHeight, layer, index)


#region Overloaded events
	def mouseMoveEvent(self, event: QMouseEvent):
		if self.mapFile is None:
			self.setStatusTip("Map not loaded")
			return

		mapSize = self.mapFile.largestLayerSize()
		tileW = self.tileWidth
		tileH = self.tileHeight
		mapRect = QRect(0, 0, mapSize.width() * tileW, mapSize.height() * tileH)
		
		tilePos = self.widgetToMapPos(event.pos().x(), event.pos().y())
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
			self.drawMode = DrawMode.MouseDown
			self.drawModeChanged.emit(self.drawMode)
			self.__drawTemporaryTile()


	def mouseReleaseEvent(self, event: QMouseEvent) -> None:
		self.drawing = False
		self.drawMode = DrawMode.MouseReleased
		self.drawModeChanged.emit(self.drawMode)
		self.drawingPoints = []


	def leaveEvent(self, event: QEvent):
		super().leaveEvent(event)
		if self.pointerGroup is not None and shiboken6.isValid(self.pointerGroup):
			self.pointerGroup.hide()
		self.drawing = False # drawing is changed, but the mouse is not released so drawMode is not changed
		self.window().setStatus("")
#endregion


	def __filterItems(self, item:QGraphicsItem):
		return isinstance(item, QGraphicsPixmapItem) and item.zValue() == self.currentLayer


	def __drawTemporaryTile(self):
		pRect = self.pointerRect(True)
		pRectWidgetUL = self.mapFromScene(self.mapToWidgetPos(pRect.x(), pRect.y()))
		pRectWidgetBR = self.mapFromScene(self.mapToWidgetPos(pRect.right(), pRect.bottom()))
		pRectWidget = QRect(pRectWidgetUL, pRectWidgetBR)
		affectedItems = self.items(pRectWidget)
		filteredItems:list[QGraphicsPixmapItem] = list(filter(self.__filterItems, affectedItems))
		currentImage = self.mapFile.tileset.tiles[self.currentTile].image
		currentUuid = self.mapFile.tileset.tiles[self.currentTile].uuid
		for item in filteredItems:
			itemPos = item.pos().toPoint()
			if itemPos in self.drawingPoints:
				continue
			self.drawingPoints.append(self.widgetToMapPos(itemPos.x(), itemPos.y()))
			tempItem = self.scene().addPixmap(QPixmap.fromImage(currentImage))
			tempItem.setPos(item.x(), item.y())
			tempItem.setZValue(self.currentLayer + 0.5)
			tempItem.setData(0, currentUuid)


	def removeTemporaryTiles(self):
		temporaryItems = list(filter(lambda i: isinstance(i, QGraphicsPixmapItem) and i.zValue() % 1 > 0, self.mapScene.items()))
		for item in temporaryItems:
			self.mapScene.removeItem(item)


	def __resetPointerGroup(self):
		if self.pointerGroup is not None and shiboken6.isValid(self.pointerGroup):
			items = self.pointerGroup.childItems()
			for item in items:
				self.mapScene.removeItem(item)
			self.mapScene.removeItem(self.pointerGroup)

		self.pointerGroup = QGraphicsItemGroup()
		self.mapScene.addItem(self.pointerGroup)


	def __resetGridGroup(self):
		if self.gridGroup is not None and shiboken6.isValid(self.gridGroup):
			if self.gridGroup.scene() is not None:
				for item in self.gridGroup.childItems():
					self.mapScene.removeItem(item)
				self.mapScene.destroyItemGroup(self.gridGroup)
			del self.gridGroup
		self.gridGroup = QGraphicsItemGroup()
		self.gridVisible = False


	def __updateGrid(self):
		self.__resetGridGroup()
		settings = Settings()
		gridColor = settings.gridColor
		if not gridColor.isValid():
			gridColor = Defaults.gridColor
			settings.gridColor = gridColor
		gridColor.setAlpha(128)

		mapSize = self.sceneRect().size()
		mapWidth = int(mapSize.width())
		mapHeight = int(mapSize.height())
		
		gridWidth = self.mapFile.tileset.tileWidth
		gridHeight = self.mapFile.tileset.tileHeight

		for y in range(gridHeight, mapHeight, gridHeight):
			line = QGraphicsLineItem(0, y, mapWidth-1, y)
			line.setPen(gridColor)
			self.gridGroup.addToGroup(line)
		
		for x in range(gridWidth, mapWidth, gridWidth):
			line = QGraphicsLineItem(x, 0, x, mapHeight-1)
			line.setPen(gridColor)
			self.gridGroup.addToGroup(line)

		self.mapScene.addItem(self.gridGroup)
		self.gridGroup.setZValue(257)


	@Slot(QPoint)
	def onCustomContextMenuRequested(self, pos:QPoint):
		if not self.sceneRect().contains(pos):
			return

		tilePos = self.widgetToMapPos(pos.x(), pos.y())
		match self.contextMenu.exec(self.mapToGlobal(pos)):
			case self.selectTileAction:
				self.currentTile = self.mapFile.getTileIndexAt(tilePos.x(), tilePos.y(), self.currentLayer)
				self.currentTileChanged.emit(self.currentTile)
			case self.setEntryPointAction:
				self.setEntryPointRequested.emit(tilePos, self.currentLayer)
			case self.newPersonAction:
				self.newPersonRequested.emit(tilePos, self.currentLayer)
			case self.newTriggerAction:
				self.newTriggerRequested.emit(tilePos, self.currentLayer)
			case self.editEntityAction:
				self.editEntityRequested.emit(tilePos, self.currentLayer)
			case self.deleteEntityAction:
				self.deleteEntityRequested.emit(tilePos, self.currentLayer)
			case self.editZoneAction:
				self.editZoneRequested.emit(tilePos, self.currentLayer)


	@Slot(QPoint)
	def onHoverTilePosChanged(self, pos:QPoint):
		pointerUL = self.mapToWidgetPos(
			pos.x() - self.drawSize // 2,
			pos.y() - self.drawSize // 2
		)
		try:
			self.pointerGroup.setPos(pointerUL)
			if pos.x() > -1 and pos.y() > -1:
				self.pointerGroup.show()
				if self.drawMode == DrawMode.MouseDown and self.drawing:
					self.__drawTemporaryTile()
			else:
				self.pointerGroup.hide()
		except:
			pass