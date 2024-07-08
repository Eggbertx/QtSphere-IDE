from PySide6.QtCore import Qt
from PySide6.QtGui import QPixmap, QIcon
from PySide6.QtWidgets import QWidget, QLabel, QTableWidgetItem, QToolButton, QHeaderView

from formats.spheremap import SphereMap, EntityType
from widgets.sphereeditor import SphereEditor

from ui.ui_mapeditor import Ui_MapEditor

class MapEditor(SphereEditor):
	ui: Ui_MapEditor
	map: SphereMap
	def __init__(self, parent: QWidget | None = None):
		super().__init__(parent)
		self.ui = Ui_MapEditor()
		self.ui.setupUi(self)
		self.map = None

		self.ui.layersTable.setColumnWidth(0,48)
		self.ui.layersTable.setColumnWidth(2,24)
		self.ui.layersTable.horizontalHeader().setSectionResizeMode(1,QHeaderView.ResizeMode.Stretch)
		self.ui.entitiesTable.horizontalHeader().setSectionResizeMode(1,QHeaderView.ResizeMode.Stretch)
		self.ui.mainSplitter.setStretchFactor(0,1)
	
	def attachMap(self, map:SphereMap):
		self.map = map
		self.ui.layersTable.clear()
		self.ui.layersTable.setRowCount(len(map.layers))
		for l in range(len(self.map.layers)):
			layer = self.map.layers[len(self.map.layers) - l - 1]
			eyeLabel = QLabel()
			eyeLabel.setToolTip("Toggle layer visibility")
			if self.map.layers[l].visible:
				eyeLabel.setPixmap(QPixmap(":/icons/res/eye.png"))
			else:
				eyeLabel.setPixmap(QPixmap(":/icons/res/eye-closed.png"))
			eyeLabel.setAlignment(Qt.AlignmentFlag.AlignCenter)
			self.ui.layersTable.setCellWidget(l,0,eyeLabel)
			self.ui.layersTable.setItem(l,1, QTableWidgetItem(layer.name))

			deleteLabel = QLabel("X")
			deleteLabel.setToolTip("Delete layer")
			deleteLabel.setStyleSheet("QLabel { color:red;font-weight:bold; }")
			deleteLabel.setAlignment(Qt.AlignmentFlag.AlignCenter)
			self.ui.layersTable.setCellWidget(l,2,deleteLabel)

		self.ui.entitiesTable.clear()
		entities = list(filter(lambda e: e.type == 1, map.entities))
		self.ui.entitiesTable.setRowCount(len(entities))
		
		for e in range(len(entities)):
			self.ui.entitiesTable.setCellWidget(e, 0, QLabel(entities[e].name))
			self.ui.entitiesTable.setCellWidget(e, 1, QLabel(entities[e].spritesetFilename))
			browseBtn = QToolButton()
			browseBtn.setText("...")
			self.ui.entitiesTable.setCellWidget(e, 2, browseBtn)
			self.ui.entitiesTable.setColumnWidth(2, browseBtn.width())
		
		for tile in map.tileset.tiles:
			self.ui.tilesetView.addPixmap(tile.image)
		
		self.ui.mapView.attachMap(map)