from PySide6.QtCore import Qt, Slot
from PySide6.QtGui import QPixmap, QIcon, QAction, QActionGroup
from PySide6.QtWidgets import QWidget, QLabel, QTableWidgetItem, QToolButton, QHeaderView, QMenu, QToolBar

from dialogs.layerpropertiesdialog import LayerPropertiesDialog
from formats.spheremap import SphereMap, EntityType, MapLayer, MapEntity
from widgets.sphereeditor import SphereEditor
from widgets.map.mapview import MapTool

from ui.ui_mapeditor import Ui_MapEditor


class MapEditor(SphereEditor):
	ui: Ui_MapEditor
	layerMenu:QMenu

	# menubar tools
	menuBar:QToolBar
	pencilMenu:QMenu
	pencil1:QAction
	pencil3:QAction
	pencil5:QAction
	pencilTool:QToolButton
	lineTool:QAction
	fillTool:QAction
	dropperTool:QAction
	currentTool:MapTool
	gridTool:QAction
	showSpritesetsTool:QAction
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
		self.menuBar = QToolBar()
		self.ui.mapViewLayout.setMenuBar(self.menuBar)

		self.pencilMenu = QMenu(self)
		self.pencil1 = self.pencilMenu.addAction(QIcon(":/res/1x1grid.png"), "1x1")
		self.pencil1.triggered.connect(lambda: self.ui.mapView.setDrawSize(1))
		self.pencil3 = self.pencilMenu.addAction(QIcon(":/res/3x3grid.png"), "3x3")
		self.pencil3.triggered.connect(lambda: self.ui.mapView.setDrawSize(3))
		self.pencil5 = self.pencilMenu.addAction(QIcon(":/res/5x5grid.png"), "5x5")
		self.pencil5.triggered.connect(lambda: self.ui.mapView.setDrawSize(5))
		self.pencilMenu.setDefaultAction(self.pencil1)
		self.pencilMenu.triggered.connect(self.setCurrentTool)

		self.pencilTool = QToolButton(self.menuBar)
		self.pencilTool.setIcon(QIcon(":/res/pencil.png"))
		self.pencilTool.setText("Pencil")
		self.pencilTool.setToolTip("Pencil")
		self.pencilTool.setMenu(self.pencilMenu)
		self.pencilTool.setPopupMode(QToolButton.ToolButtonPopupMode.MenuButtonPopup)
		self.pencilTool.setCheckable(True)
		self.pencilTool.setChecked(True)
		self.menuBar.addWidget(self.pencilTool)
		self.pencilTool.clicked.connect(lambda: self.setCurrentTool(self.pencilTool))

		self.lineTool = self.menuBar.addAction(QIcon(":/res/linetool.png"), "Line")
		self.lineTool.setCheckable(True)
		self.rectTool = self.menuBar.addAction(QIcon(":/res/rectangletool.png"), "Rectangle")
		self.rectTool.setCheckable(True)
		self.fillTool = self.menuBar.addAction(QIcon(":/res/paintbucket.png"), "Fill layer")
		self.fillTool.setCheckable(True)
		self.dropperTool = self.menuBar.addAction(QIcon(":/res/dropper.png"), "Select tile")
		self.dropperTool.setCheckable(True)
		self.menuBar.addSeparator()
		self.gridTool = self.menuBar.addAction(QIcon(":/res/togglegrid.png"), "Show/Hide grid")
		self.gridTool.setCheckable(True)
		self.showSpritesetsTool = self.menuBar.addAction(QIcon(":/res/show_spritesets.png"), "Show/hide spritesets (not yet implemented)")
		self.showSpritesetsTool.setCheckable(True)
		self.menuBar.actionTriggered.connect(self.setCurrentTool)
		self.ui.tilesetView.indexChanged.connect(self.ui.mapView.onTileIndexChanged)

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
		eyeLabel = QLabel("Toggle visible")
		eyeLabel.setToolTip("Toggle layer visibility")
		eyeLabel.setPixmap(QPixmap(":/res/eye.png" if layer.visible else ":/res/eye-closed.png"))
		eyeLabel.setAlignment(Qt.AlignmentFlag.AlignCenter)
		l = self.ui.layersTable.rowCount() - 1
		self.ui.layersTable.setCellWidget(l,0,eyeLabel)
		self.ui.layersTable.setItem(l,1, QTableWidgetItem(layer.name))
		self.ui.layersTable.cellClicked.emit(0, 1)
		self.ui.layersTable.selectRow(0)

		deleteLabel = QLabel("X")
		deleteLabel.setToolTip("Delete layer")
		deleteLabel.setStyleSheet("QLabel { color:red;font-weight:bold; }")
		deleteLabel.setAlignment(Qt.AlignmentFlag.AlignCenter)
		self.ui.layersTable.setCellWidget(l,2,deleteLabel)

	def attachEntity(self, entity:MapEntity):
		e = self.ui.entitiesTable.rowCount() - 1
		self.ui.entitiesTable.setCellWidget(e, 0, QLabel(entity.name))
		self.ui.entitiesTable.setCellWidget(e, 1, QLabel(entity.spritesetFilename))
		browseBtn = QToolButton()
		browseBtn.setText("...")
		self.ui.entitiesTable.setCellWidget(e, 2, browseBtn)
		self.ui.entitiesTable.setColumnWidth(2, browseBtn.width())

	def attachMap(self, map:SphereMap):
		self.ui.mapView.attachMap(map)
		self.ui.layersTable.clear()
		self.ui.layersTable.setRowCount(len(map.layers))
		for l in range(len(self.map.layers)):
			layer = self.map.layers[len(self.map.layers) - l - 1] # layers are ordered bottom to top, get them in reverse order
			self.attachLayer(layer)

		self.ui.layersTable.cellClicked.emit(0, 1)
		self.ui.layersTable.selectRow(0)

		self.ui.entitiesTable.clear()
		entities = list(filter(lambda e: e.type == 1, map.entities))
		self.ui.entitiesTable.setRowCount(len(entities))
		
		for e in range(len(entities)):
			self.attachEntity(entities[e])
		
		for tile in map.tileset.tiles:
			self.ui.tilesetView.addPixmap(tile.image)


	@Slot(int,int)
	def onLayerTableCellClicked(self, row:int, column:int):
		if column == 0:
			# clicked eye
			item:QLabel = self.ui.layersTable.cellWidget(row, column)
			visible = self.ui.mapView.toggleLayerVisibility(self.ui.layersTable.rowCount() - row - 1)
			item.setPixmap(QPixmap(":/res/eye.png" if visible else ":/res/eye-closed.png"))
		elif column == 2:
			# clicked remove
			if self.ui.layersTable.rowCount() > 1:
				self.ui.mapView.deleteLayer(self.ui.layersTable.rowCount() - row - 1)
				self.ui.layersTable.removeRow(row)
				return
		self.ui.mapView.currentLayer = row


	@Slot()
	def layerPropertiesRequested(self):
		self.layerPropertiesDialog.show(self.ui.layersTable.currentRow(), self.map)


	@Slot(QAction)
	def setCurrentTool(self, tool:QAction|QToolButton):
		if tool == self.gridTool:
			self.ui.mapView.gridVisible = self.gridTool.isChecked()
			return
		elif tool == self.showSpritesetsTool:
			return

		self.pencilTool.setChecked(False)
		self.lineTool.setChecked(False)
		self.rectTool.setChecked(False)
		self.fillTool.setChecked(False)
		self.dropperTool.setChecked(False)

		match tool:
			case self.pencil1|self.pencil3|self.pencil5|self.pencilTool:
				self.ui.mapView.setCurrentTool(MapTool.Pencil)
				self.pencilTool.setChecked(True)
			case self.lineTool:
				self.ui.mapView.setCurrentTool(MapTool.Line)
				self.lineTool.setChecked(True)
			case self.rectTool:
				self.ui.mapView.setCurrentTool(MapTool.Rectangle)
				self.rectTool.setChecked(True)
			case self.fillTool:
				self.ui.mapView.setCurrentTool(MapTool.Fill)
				self.fillTool.setChecked(True)
			case self.dropperTool:
				self.ui.mapView.setCurrentTool(MapTool.Select)
				self.dropperTool.setChecked(True)
