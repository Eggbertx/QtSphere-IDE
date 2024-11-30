from PySide6.QtCore import Signal, Slot, Qt
from PySide6.QtGui import QPixmap, QIcon
from PySide6.QtWidgets import QHeaderView, QWidget, QTableWidget, QPushButton, QTableWidgetItem

from formats.spheremap import MapLayer, SphereMap
from widgets.map.mapview import MapView


class LayersTable(QTableWidget):
	mapView:MapView

	layerVisibilityToggleRequested:Signal = Signal(int)
	insertLayerRequested:Signal = Signal(int)
	deleteLayerRequested:Signal = Signal(int)
	duplicateLayerRequested:Signal = Signal(int)
	moveLayerUpRequested:Signal = Signal(int)
	moveLayerDownRequested:Signal = Signal(int)
	toggleLockLayerRequested:Signal = Signal(int)
	layerPropertiesRequested:Signal = Signal(int)
	layerRenamed:Signal = Signal(int,int,str)


	def __init__(self, parent:QWidget|None = None):
		super().__init__(1, 3, parent)
		self.setColumnCount(3)
		self.setRowCount(0)
		self.horizontalHeader().setVisible(False)
		self.verticalHeader().setVisible(False)
		self.horizontalHeader().setSectionResizeMode(1, QHeaderView.ResizeMode.Stretch)
		self.horizontalHeader().setSectionResizeMode(1, QHeaderView.ResizeMode.Stretch)
		self.setColumnWidth(0, 48)
		self.setColumnWidth(2, 24)
		self.cellClicked.connect(self.onCellClicked)
		self.setContextMenuPolicy(Qt.ContextMenuPolicy.ActionsContextMenu)
		self.setupContextMenu()
		self.setSelectionMode(QTableWidget.SelectionMode.SingleSelection)


	def setupContextMenu(self):
		self.addAction("Insert Layer", lambda: self.insertLayerRequested.emit(self.currentLayer()))
		self.addAction("Delete Layer", lambda: self.deleteLayerRequested.emit(self.currentLayer()))
		self.deleteLayerRequested.connect(self.onDeleteLayerRequested)
		self.addAction("Duplicate Layer", lambda: self.duplicateLayerRequested.emit(self.currentLayer()))
		self.addAction("").setSeparator(True)
		self.addAction("Move Layer Up", lambda: self.moveLayerUpRequested.emit(self.currentLayer()))
		self.addAction("Move Layer Down", lambda: self.moveLayerDownRequested.emit(self.currentLayer()))
		self.addAction("").setSeparator(True)
		self.addAction("Toggle Lock Layer", lambda: self.toggleLockLayerRequested.emit(self.currentLayer()))
		self.addAction("Properties", lambda: self.layerPropertiesRequested.emit(self.currentLayer()))


	def rowToLayer(self, row:int) -> int:
		return self.rowCount() - row - 1


	def currentLayer(self):
		return self.rowToLayer(self.currentRow())


	def attachMap(self, map:SphereMap):
		self.clear()
		for l in range(len(map.layers)-1, -1, -1):
			# layers are ordered bottom to top, get them in reverse order
			layer = map.layers[l]
			self.attachLayer(layer)

		self.cellClicked.emit(0, 1)
		self.selectRow(0)
		self.cellChanged.connect(self.onCellChanged)


	def insertLayer(self, layer:MapLayer, row:int):
		self.insertRow(row)

		eyeButton = QPushButton("")
		eyeButton.setIcon(QPixmap(":/res/eye.png" if layer.visible else ":/res/eye-closed.png"))
		eyeButton.setFlat(True)
		eyeButton.setToolTip("Toggle layer visibility")
		eyeButton.clicked.connect(self.onToggleLayerVisibleButtonClicked)
		self.setCellWidget(row, 0, eyeButton)

		self.setItem(row,1, QTableWidgetItem(layer.name))
		self.cellClicked.emit(0, 1)

		deleteButton = QPushButton(QIcon.fromTheme("list-remove"), "")
		deleteButton.setFlat(True)
		deleteButton.setToolTip("Delete layer")
		deleteButton.clicked.connect(self.onDeleteLayerButtonClicked)
		self.setCellWidget(row, 2, deleteButton)


	def attachLayer(self, layer:MapLayer):
		l = self.rowCount()
		self.insertLayer(layer, l)


	def getWidgetRow(self, btn:QWidget):
		return self.rowAt(btn.pos().y())


	@Slot(int,int)
	def onCellClicked(self, row:int, column:int):
		self.mapView.currentLayer = row


	@Slot(bool)
	def onToggleLayerVisibleButtonClicked(self):
		row = self.getWidgetRow(self.sender())
		layer = self.rowToLayer(row)
		self.layerVisibilityToggleRequested.emit(layer)
		btn:QPushButton = self.cellWidget(row, 0)
		visible = self.mapView.isLayerVisible(layer)
		btn.setIcon(QPixmap(":/res/eye.png" if visible else ":/res/eye-closed.png"))


	@Slot(int,int)
	def onCellChanged(self, row:int, col:int):
		if col != 1:
			return
		self.layerRenamed.emit(row, col, self.item(row, col).text())


	@Slot(int)
	def onDeleteLayerRequested(self, layer:int):
		if self.rowCount() <= 1:
			return
		row = self.rowCount() - layer - 1
		self.removeRow(row)


	@Slot(bool)
	def onDeleteLayerButtonClicked(self):
		if self.rowCount() <= 1:
			return
		row = self.getWidgetRow(self.sender())
		self.deleteLayerRequested.emit(self.rowToLayer(row))
