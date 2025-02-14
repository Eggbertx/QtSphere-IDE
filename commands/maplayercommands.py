from PySide6.QtGui import QUndoCommand

from commands.commandids import CommandIDs
from formats.spheremap import SphereMap
from widgets.map.layerstable import LayersTable
from widgets.map.mapview import MapView

class LayerRenamedCommand(QUndoCommand):
	map:SphereMap
	table:LayersTable
	layer:int
	oldName:str
	newName:str

	def __init__(self, map:SphereMap, table:LayersTable, layer:int):
		super().__init__()
		self.map = map
		self.table = table
		self.layer = layer
		self.oldName = map.layers[layer].name
		self.newName = table.item(table.layerToRow(layer), 1).text()


	def id(self) -> int:
		return CommandIDs.RenameMapLayer.value


	def undo(self) -> None:
		self.map.layers[self.layer].name = self.oldName
		state = self.table.blockSignals(True)
		self.table.item(self.table.layerToRow(self.layer), 1).setText(self.oldName)
		self.table.blockSignals(state)



	def redo(self) -> None:
		self.map.layers[self.layer].name = self.newName
		item = self.table.item(self.table.layerToRow(self.layer), 1)
		if item.text() != self.newName:
			state = self.table.blockSignals(True)
			item.setText(self.newName)
			self.table.blockSignals(state)


	def mergeWith(self, other: QUndoCommand) -> bool:
		if other.id() == self.id() and \
			isinstance(other, LayerRenamedCommand) and \
			other.newName == self.oldName and \
			other.layer == self.layer and \
			self.newName != self.oldName and \
			self.map.layers[self.layer].name != other.newName:
			self.setText("Rename tile")
			return True
		return False


class LayerVisibilityToggleCommand(QUndoCommand):
	map:SphereMap
	mapView:MapView
	table:LayersTable
	layer:int
	visible:bool

	def __init__(self, map:SphereMap, mapView:MapView, table:LayersTable, layer:int, visible:bool):
		super().__init__()
		self.map = map
		self.mapView = mapView
		self.table = table
		self.layer = layer
		self.visible = visible


	def id(self) -> int:
		return CommandIDs.ToggleMapLayerVisibility.value


	def undo(self) -> None:
		self.map.layers[self.layer].visible = self.visible
		self.mapView.setLayerVisible(self.layer, self.visible)
		self.table.updateButtonPixmap(self.layer)


	def redo(self) -> None:
		self.map.layers[self.layer].visible = not self.visible
		self.mapView.setLayerVisible(self.layer, not self.visible)
		self.table.updateButtonPixmap(self.layer)


	def mergeWith(self, other: QUndoCommand) -> bool:
		if other.id() == self.id() and \
			isinstance(other, LayerVisibilityToggleCommand) and \
			other.layer == self.layer and \
			other.visible == self.visible:
			self.setText("Toggle layer visibility")
			return True
		return False