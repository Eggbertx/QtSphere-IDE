from PySide6.QtCore import Qt, Slot
from PySide6.QtGui import QPixmap, QIcon, QAction, QActionGroup
from PySide6.QtWidgets import QWidget, QLabel, QTableWidgetItem, QToolButton, QHeaderView, QMenu, QPushButton, QTableWidget, QStyle

from dialogs.layerpropertiesdialog import LayerPropertiesDialog
from formats.spheremap import SphereMap, EntityType, MapLayer, MapEntity
from widgets.sphereeditor import SphereEditor
from widgets.map.mapview import MapTool
from widgets.image.drawingtoolbar import DrawingToolbar

from ui.ui_mapeditor import Ui_MapEditor


class MapEditor(SphereEditor):
	ui: Ui_MapEditor
	layerMenu:QMenu

	# menubar tools
	menuBar:DrawingToolbar
	toggleGridAction:QAction
	toggleSpritesetsAction:QAction
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
		self.toggleSpritesetsAction = self.menuBar.addAction("Show Spritesets")
		self.toggleSpritesetsAction.setIcon(QIcon(":/res/show_spritesets.png"))
		self.toggleSpritesetsAction.setCheckable(True)
		self.toggleSpritesetsAction.setChecked(False)
		self.menuBar.actionTriggered.connect(self.setCurrentTool)

	def setupContextMenus(self):
		self.layerMenu = QMenu(self)
		self.layerMenu.addAction("Insert layer")
		self.layerMenu.addAction("Delete layer")
		self.layerMenu.addAction("Duplicate layer")
		self.layerMenu.addSeparator()
		self.layerMenu.addAction("Toggle lock layer")
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
		browseBtn = QToolButton()
		browseBtn.setText("...")
		self.ui.entitiesTable.setCellWidget(e, 2, browseBtn)
		self.ui.entitiesTable.setColumnWidth(2, browseBtn.width())

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
		
		for tile in map.tileset.tiles:
			self.ui.tilesetView.addPixmap(tile.image)


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
	def layerPropertiesRequested(self):
		self.layerPropertiesDialog.show(self.ui.layersTable.currentRow(), self.map)


	@Slot(QAction)
	def setCurrentTool(self, tool:QAction|QToolButton):
		if tool == self.toggleGridAction:
			self.ui.mapView.gridVisible = self.toggleGridAction.isChecked()
			return
		elif tool == self.toggleSpritesetsAction:
			return

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
