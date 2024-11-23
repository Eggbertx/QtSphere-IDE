from PySide6.QtCore import Slot
from PySide6.QtGui import QPixmap, QIcon, QAction
from PySide6.QtWidgets import QWidget, QLabel, QTableWidgetItem, QToolButton, QHeaderView, QMenu, QPushButton

from commands.tilesetcommands import TilesetInsertTileCommand
from dialogs.layerpropertiesdialog import LayerPropertiesDialog
from formats.spheremap import SphereMap, EntityType, MapLayer, MapEntity
from widgets.sphereeditor import SphereEditor
from widgets.map.mapview import MapTool
from widgets.image.drawingtoolbar import DrawingToolbar

from ui.ui_mapeditor import Ui_MapEditor


class MapEditor(SphereEditor):
	ui: Ui_MapEditor
	layerMenu:QMenu

	menuBar:DrawingToolbar
	toggleGridAction:QAction

	layerPropertiesDialog:LayerPropertiesDialog

	@property
	def map(self):
		return self.ui.mapView.mapFile

	@staticmethod
	def openAndAttach(parent: QWidget, filePath:str):
		rmp = SphereMap(filePath)
		rmp.open()
		editor = MapEditor(parent)
		editor.attachMap(rmp)
		return editor


	def __init__(self, parent: QWidget | None = None):
		super().__init__(parent)
		self.ui = Ui_MapEditor()
		self.ui.setupUi(self)
		self.setupToolbar()
		self.setupContextMenus()
		self.currentTool = MapTool.Pencil
		self.layerPropertiesDialog = LayerPropertiesDialog(self)
		self.ui.layersTable.setColumnWidth(0,48)
		self.ui.layersTable.setColumnWidth(2,24)
		self.ui.layersTable.horizontalHeader().setSectionResizeMode(1,QHeaderView.ResizeMode.Stretch)
		self.ui.entitiesTable.horizontalHeader().setSectionResizeMode(1,QHeaderView.ResizeMode.Stretch)
		self.ui.mainSplitter.setStretchFactor(0,1)
		self.ui.layersTable.cellClicked.connect(self.onLayerTableCellClicked)
		self.ui.tilesetView.tilesInserted.connect(self.onTilesetTilesInserted)
		self.ui.tilesetView.tilesAppended.connect(self.onTilesetTilesAppended)
		self.ui.tilesetView.tilesRemoved.connect(self.onTilesetTilesRemoved)


	def setupToolbar(self):
		self.menuBar = DrawingToolbar()
		self.ui.mapViewLayout.setMenuBar(self.menuBar)

		self.menuBar.pencil1.triggered.connect(lambda: self.ui.mapView.setDrawSize(1))
		self.menuBar.pencil3.triggered.connect(lambda: self.ui.mapView.setDrawSize(3))
		self.menuBar.pencil5.triggered.connect(lambda: self.ui.mapView.setDrawSize(5))
		self.menuBar.pencilMenu.triggered.connect(self.setCurrentTool)
		self.menuBar.pencilTool.clicked.connect(lambda: self.setCurrentTool(self.menuBar.pencilTool))
		self.menuBar.addSeparator()
		self.toggleGridAction = self.menuBar.addCheckableAction(QIcon(":/res/togglegrid.png"), "Show/Hide grid", True)
		self.ui.tilesetView.indexChanged.connect(self.ui.mapView.onTileIndexChanged)

		mapGraphicsMenuButton = QToolButton()
		mapGraphicsMenuButton.setIcon(QIcon(":/res/person.svg"))
		mapGraphicsMenuButton.setToolTip("Map icons")
		mapGraphicsMenuButton.setPopupMode(QToolButton.ToolButtonPopupMode.InstantPopup)
		spAction = self.addButtonMenuItem(mapGraphicsMenuButton, QIcon(":/res/spawnpoint_icon.svg"),
			"Show spawn point", True)
		spAction.triggered.connect(lambda: self.onMapGraphicToggleTriggered(0, spAction.isChecked()))

		personsAction = self.addButtonMenuItem(mapGraphicsMenuButton, QIcon(":/res/person.svg"),
			"Show person entities", True)
		personsAction.triggered.connect(lambda: self.onMapGraphicToggleTriggered(1, personsAction.isChecked()))

		triggersAction = self.addButtonMenuItem(mapGraphicsMenuButton, QIcon(":/res/trigger.svg"),
			"Show trigger entities", True)
		triggersAction.triggered.connect(lambda: self.onMapGraphicToggleTriggered(2, triggersAction.isChecked()))

		self.menuBar.addWidget(mapGraphicsMenuButton)
		self.menuBar.notToolActions.append(mapGraphicsMenuButton)

		self.menuBar.actionTriggered.connect(self.setCurrentTool)


	def addButtonMenuItem(self, btn:QToolButton, icon:QIcon, text:str, checked:bool) -> QAction:
		action = btn.addAction(icon, text)
		action.setCheckable(True)
		action.setChecked(checked)
		return action


	def setupContextMenus(self):
		self.layerMenu = QMenu(self)
		self.layerMenu.addAction("Insert layer", self.onInsertLayerTriggered)
		self.layerMenu.addAction("Delete layer", self.deleteCurrentLayer)
		self.layerMenu.addAction("Duplicate layer", self.duplicateCurrentLayer)
		self.layerMenu.addSeparator()
		self.layerMenu.addAction("Move layer up", self.moveCurrentLayerUp)
		self.layerMenu.addAction("Move layer down", self.moveCurrentLayerDown)
		self.layerMenu.addSeparator()
		self.layerMenu.addAction("Toggle lock layer", self.onLockCurrentLayerTriggered)
		self.layerMenu.addAction("Properties", self.layerPropertiesRequested)
		self.ui.layersTable.customContextMenuRequested.connect(
			lambda pos: self.layerMenu.exec(self.ui.layersTable.mapToGlobal(pos)))


	def attachLayer(self, layer:MapLayer):
		l = self.ui.layersTable.rowCount()
		self.ui.layersTable.insertRow(l)

		eyeButton = QPushButton("")
		eyeButton.setIcon(QPixmap(":/res/eye.png" if layer.visible else ":/res/eye-closed.png"))
		eyeButton.setFlat(True)
		eyeButton.setToolTip("Toggle layer visibility")
		eyeButton.clicked.connect(self.onToggleLayerVisibleButtonClicked)
		self.ui.layersTable.setCellWidget(l,0,eyeButton)

		self.ui.layersTable.setItem(l,1, QTableWidgetItem(layer.name))
		self.ui.layersTable.cellClicked.emit(0, 1)
		self.ui.layersTable.selectRow(0)

		deleteButton = QPushButton(QIcon.fromTheme("list-remove"), "")
		deleteButton.setFlat(True)
		deleteButton.setToolTip("Delete layer")
		deleteButton.clicked.connect(self.onDeleteLayerButtonClicked)
		self.ui.layersTable.setCellWidget(l,2,deleteButton)


	def getTableWidgetRow(self, btn:QWidget):
		return self.ui.layersTable.rowAt(btn.pos().y())


	def attachEntity(self, entity:MapEntity):
		e = self.ui.entitiesTable.rowCount()
		self.ui.entitiesTable.insertRow(e)
		self.ui.entitiesTable.setCellWidget(e, 0, QLabel(entity.name))
		self.ui.entitiesTable.setCellWidget(e, 1, QLabel(entity.spritesetFilename))
		propertiesButton = QPushButton(QIcon.fromTheme(QIcon.ThemeIcon.DocumentProperties), "")
		propertiesButton.setFlat(True)
		
		self.ui.entitiesTable.setCellWidget(e, 2, propertiesButton)
		self.ui.entitiesTable.setColumnWidth(2, propertiesButton.width())


	def updateTilesetTitle(self):
		self.ui.tilesetBox.setTitle("Tiles (%d)" % self.ui.tilesetView.numTiles)


	def attachMap(self, map:SphereMap):
		self.ui.mapView.attachMap(map)
		self.ui.layersTable.clear()
		for l in range(len(self.map.layers)):
			layer = self.map.layers[len(self.map.layers) - l - 1] # layers are ordered bottom to top, get them in reverse order
			self.attachLayer(layer)

		self.ui.layersTable.cellClicked.emit(0, 1)
		self.ui.layersTable.selectRow(0)

		self.ui.entitiesTable.clear()
		entities = list(filter(lambda e: e.type == 1, map.entities))
		
		for e in range(len(entities)):
			self.attachEntity(entities[e])
		
		self.ui.tilesetView.attachTileset(map.tileset)
		self.ui.tilesetView.mapEditor = self
		self.updateTilesetTitle()


	@Slot(bool)
	def onToggleLayerVisibleButtonClicked(self):
		row = self.getTableWidgetRow(self.sender())
		btn:QPushButton = self.ui.layersTable.cellWidget(row, 0)
		visible = self.ui.mapView.toggleLayerVisibility(self.ui.layersTable.rowCount() - row - 1)
		btn.setIcon(QPixmap(":/res/eye.png" if visible else ":/res/eye-closed.png"))


	@Slot(bool)
	def onDeleteLayerButtonClicked(self):
		row = self.getTableWidgetRow(self.sender())
		self.ui.mapView.deleteLayer(self.ui.layersTable.rowCount() - row - 1)
		self.ui.layersTable.removeRow(row)


	@Slot(int,int)
	def onLayerTableCellClicked(self, row:int, column:int):
		self.ui.mapView.currentLayer = row


	@Slot()
	def onInsertLayerTriggered(self):
		pass


	@Slot()
	def duplicateCurrentLayer(self):
		pass


	@Slot()
	def moveCurrentLayerUp(self):
		pass


	@Slot()
	def moveCurrentLayerDown(self):
		pass


	@Slot()
	def deleteCurrentLayer(self):
		row = self.ui.layersTable.currentRow()
		self.ui.mapView.deleteLayer(self.ui.layersTable.rowCount() - row - 1)
		self.ui.layersTable.removeRow(row)


	@Slot()
	def onLockCurrentLayerTriggered(self):
		pass


	@Slot()
	def layerPropertiesRequested(self):
		self.layerPropertiesDialog.show(self.ui.layersTable.currentRow(), self.map)


	@Slot(int,bool)
	def onMapGraphicToggleTriggered(self, which:int, show:bool):
		match which:
			case 0:
				self.ui.mapView.spawnPointIconVisible = show
			case EntityType.Person:
				self.ui.mapView.personIconsVisible = show
			case EntityType.Trigger:
				self.ui.mapView.triggerIconsVisible = show


	@Slot(QAction)
	def setCurrentTool(self, tool:QAction|QToolButton):
		match tool:
			case self.menuBar.pencil1|self.menuBar.pencil3|self.menuBar.pencil5|self.menuBar.pencilTool:
				self.ui.mapView.setCurrentTool(MapTool.Pencil)
			case self.menuBar.lineTool:
				self.ui.mapView.setCurrentTool(MapTool.Line)
			case self.menuBar.rectTool:
				self.ui.mapView.setCurrentTool(MapTool.Rectangle)
			case self.menuBar.fillTool:
				self.ui.mapView.setCurrentTool(MapTool.Fill)
			case self.menuBar.dropperTool:
				self.ui.mapView.setCurrentTool(MapTool.Select)
			case self.toggleGridAction:
				self.ui.mapView.gridVisible = self.toggleGridAction.isChecked()


	@Slot(int,int)
	def onTilesetTilesInserted(self, index:int, count:int):
		self.updateTilesetTitle()
		self.undoStack.push(TilesetInsertTileCommand(self.ui.tilesetView, self.ui.tilesetView.numTiles, index, count))


	@Slot(int)
	def onTilesetTilesAppended(self, count:int):
		self.updateTilesetTitle()


	@Slot(int,int)
	def onTilesetTilesRemoved(self, index:int, count:int):
		self.updateTilesetTitle()

