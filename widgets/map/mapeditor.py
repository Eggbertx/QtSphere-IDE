from PySide6.QtCore import Slot, Signal, QSize
from PySide6.QtGui import QIcon, QAction
from PySide6.QtWidgets import QWidget, QLabel, QToolButton, QMenu, QPushButton

from commands.maplayerresizecommands import ResizeAllMapLayersCommand, ResizeCurrentMapLayerCommand
from commands.mappropertiescommands import MapPropertiesChangedCommand
from commands.mapviewcommands import PencilDrawMapCommand
from commands.tilesetcommands import TileSizeChangedCommand, TilesetInsertTilesCommand, TilesetAppendTilesCommand, TilesetRemoveTilesCommand
from commands.maplayercommands import LayerRenamedCommand, LayerVisibilityToggleCommand
from dialogs.layerpropertiesdialog import LayerPropertiesDialog
from dialogs.tileepropertiesdialog import TilePropertiesDialog
from formats.spheremap import SphereMap, EntityType, MapEntity
from formats.tileset import Tileset
from widgets.sphereeditor import SphereEditor, SphereEditorType
from widgets.image.drawingtoolbar import DrawingToolbar

from ui.ui_mapeditor import Ui_MapEditor

from widgets.drawingenums import DrawingTool, DrawMode
from dialogs.mappropertiesdialog import MapPropertiesDialog

class MapEditor(SphereEditor):
	ui: Ui_MapEditor
	layerMenu:QMenu

	menuBar:DrawingToolbar
	toggleGridAction:QAction

	layerPropertiesDialog:LayerPropertiesDialog
	tilePropertiesDialog:TilePropertiesDialog
	mapPropertiesChanged:Signal = Signal(MapPropertiesDialog)
	allLayersResized:Signal = Signal(QSize)
	currentLayerResized:Signal = Signal(int,QSize)
	tileSizeChanged:Signal = Signal(QSize)
	tilesetRescaled:Signal = Signal(QSize)

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
		super().__init__(parent, SphereEditorType.Map)
		self.ui = Ui_MapEditor()
		self.ui.setupUi(self)
		self.setupToolbar()
		self.currentTool = DrawingTool.Pencil
		self.layerPropertiesDialog = LayerPropertiesDialog(self)
		self.tilePropertiesDialog = TilePropertiesDialog(self)
		self.ui.layersTable.mapView = self.ui.mapView
		self.ui.layersTable.layerVisibilityToggleRequested.connect(self.onLayerVisibilityToggled)
		self.ui.layersTable.deleteLayerRequested.connect(self.onDeleteLayerRequested)
		self.ui.layersTable.layerPropertiesRequested.connect(self.onLayerPropertiesDialogRequested)
		self.ui.layersTable.layerRenamed.connect(self.onLayerRenamed)
		self.ui.mainSplitter.setStretchFactor(0,1)
		self.ui.mapView.currentTileChanged.connect(self.onMapCurrentTileChanged)
		self.ui.mapView.drawModeChanged.connect(self.onDrawModeChanged)
		self.ui.tilesetView.indexChanged.connect(self.onTilesetCurrentIndexChanged)
		self.ui.tilesetView.tilesInserted.connect(self.onTilesetTilesInserted)
		self.ui.tilesetView.tilesAppended.connect(self.onTilesetTilesAppended)
		self.ui.tilesetView.tilesRemoved.connect(self.onTilesetTilesRemoved)
		self.ui.tilesetView.tilePropertiesRequested.connect(self.onTilesetTilePropertiesRequested)
		self.mapPropertiesChanged.connect(self.onMapPropertiesChanged)
		self.allLayersResized.connect(self.onAllLayersResized)
		self.currentLayerResized.connect(self.onCurrentLayerResized)
		self.tileSizeChanged.connect(self.onTileSizeChanged)
		self.tilesetRescaled.connect(self.onTilesetRescaled)


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
		self.ui.layersTable.attachMap(self.map)

		self.ui.entitiesTable.clear()
		entities = list(filter(lambda e: e.type == 1, map.entities))
		
		for e in range(len(entities)):
			self.attachEntity(entities[e])
		self.ui.tilesetView.attachTileset(map.tileset, True)
		self.ui.tilesetView.mapEditor = self
		self.updateTilesetTitle()


	def attachTileset(self, tileset:Tileset):
		self.ui.tilesetView.attachTileset(tileset, True)
		self.ui.tilesetView.mapEditor = self
		self.updateTilesetTitle()
		self.ui.mapView.attachTileset(tileset)


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
				self.ui.mapView.setCurrentTool(DrawingTool.Pencil)
			case self.menuBar.lineTool:
				self.ui.mapView.setCurrentTool(DrawingTool.Line)
			case self.menuBar.rectTool:
				self.ui.mapView.setCurrentTool(DrawingTool.Rectangle)
			case self.menuBar.fillTool:
				self.ui.mapView.setCurrentTool(DrawingTool.Fill)
			case self.menuBar.dropperTool:
				self.ui.mapView.setCurrentTool(DrawingTool.Select)
			case self.toggleGridAction:
				self.ui.mapView.gridVisible = self.toggleGridAction.isChecked()


	@Slot(MapPropertiesDialog)
	def onMapPropertiesChanged(self, dialog:MapPropertiesDialog):
		self.undoStack.push(MapPropertiesChangedCommand(dialog, self.map, self))


	@Slot(QSize)
	def onAllLayersResized(self, newSize:QSize):
		self.undoStack.push(ResizeAllMapLayersCommand(self.ui.mapView, newSize))


	@Slot(int,QSize)
	def onCurrentLayerResized(self, layer:int, newSize:QSize):
		self.undoStack.push(ResizeCurrentMapLayerCommand(self.ui.mapView, layer, newSize))


	@Slot(QSize)
	def onTileSizeChanged(self, newSize:QSize):
		self.undoStack.push(TileSizeChangedCommand(self.ui.mapView, self.ui.tilesetView, newSize, False))


	@Slot(QSize)
	def onTilesetRescaled(self, newSize:QSize):
		self.undoStack.push(TileSizeChangedCommand(self.ui.mapView, self.ui.tilesetView, newSize, True))


#region LayersTable slots
	@Slot(int)
	def onLayerVisibilityToggled(self, layer:int):
		self.undoStack.push(LayerVisibilityToggleCommand(self.map, self.ui.mapView, self.ui.layersTable, layer, self.map.layers[layer].visible))


	@Slot(int)
	def onInsertLayerRequested(self, l:int):
		pass


	@Slot(int)
	def onDeleteLayerRequested(self, layer:int):
		self.ui.mapView.deleteLayer(layer)


	@Slot(int)
	def onDuplicateLayerRequested(self, l:int):
		pass


	@Slot(int)
	def onMoveLayerUpRequested(self, l:int):
		pass


	@Slot(int)
	def onMoveLayerDownRequested(self, l:int):
		pass


	@Slot(int)
	def onLayerPropertiesDialogRequested(self, index:int):
		self.layerPropertiesDialog.show(index, self.map)


	@Slot(int,str)
	def onLayerRenamed(self, layer:int, newName:str):
		self.undoStack.push(LayerRenamedCommand(self.map, self.ui.layersTable, layer))

#endregion


#region MapView slots
	@Slot(int)
	def onMapCurrentTileChanged(self, newIndex:int):
		self.ui.tilesetView.selectedIndex = newIndex
		self.ui.tilesetView.arrangeItems()


	@Slot(DrawMode)
	def onDrawModeChanged(self, mode:DrawMode):
		if mode == DrawMode.MouseReleased:
			self.undoStack.push(PencilDrawMapCommand(self.ui.mapView.drawingPoints, self.ui.mapView.currentTile, self.ui.mapView.currentLayer, self.ui.mapView))

#endregion


#region TilesetView slots
	@Slot(int)
	def onTilesetCurrentIndexChanged(self, newIndex:int):
		self.ui.mapView.currentTile = newIndex


	@Slot(int,int)
	def onTilesetTilesInserted(self, index:int, count:int):
		self.undoStack.push(TilesetInsertTilesCommand(self.ui.tilesetView, index, count))
		self.updateTilesetTitle()


	@Slot(int)
	def onTilesetTilesAppended(self, count:int):
		self.undoStack.push(TilesetAppendTilesCommand(self.ui.tilesetView, count))
		self.updateTilesetTitle()


	@Slot(int,int)
	def onTilesetTilesRemoved(self, index:int, count:int):
		self.undoStack.push(TilesetRemoveTilesCommand(self.ui.tilesetView, index, count))
		self.updateTilesetTitle()


	@Slot(int)
	def onTilesetTilePropertiesRequested(self, index:int):
		self.tilePropertiesDialog.show(self.map.tileset.tiles[index])


	@Slot(int)
	def onToggleLockLayerRequested(self, l:int):
		pass
#endregion


