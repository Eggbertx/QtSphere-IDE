from PySide6.QtGui import QUndoCommand

from commands.commandids import CommandIDs
from formats.spheremap import SphereMap
from widgets.map.layerstable import LayersTable

class LayerRenamedCommand(QUndoCommand):
	map:SphereMap
	table:LayersTable
	row:int
	col:int
	oldName:str
	newName:str

	def __init__(self, map:SphereMap, table:LayersTable, row:int, col:int):
		super().__init__()
		self.map = map
		self.table = table
		self.row = row
		self.col = col
		self.oldName = map.layers[table.rowToLayer(row)].name
		self.newName = table.item(row, col).text()


	def id(self) -> int:
		return CommandIDs.RenameMapLayer.value


	def undo(self) -> None:
		self.map.layers[self.table.rowToLayer(self.row)].name = self.oldName
		state = self.table.blockSignals(True)
		self.table.item(self.row, self.col).setText(self.oldName)
		self.table.blockSignals(state)



	def redo(self) -> None:
		self.map.layers[self.table.rowToLayer(self.row)].name = self.newName
		item = self.table.item(self.row, self.col)
		if item.text() != self.newName:
			state = self.table.blockSignals(True)
			item.setText(self.newName)
			self.table.blockSignals(state)


	def mergeWith(self, other: QUndoCommand) -> bool:
		if other.id() == self.id() and \
			isinstance(other, LayerRenamedCommand) and \
			other.newName == self.oldName and \
			other.row == self.row and \
			other.col == self.col and \
			self.newName != self.oldName and \
			self.map.layers[self.table.rowToLayer(self.row)].name != other.newName:
			self.setText("Rename tile")
			return True
		return False