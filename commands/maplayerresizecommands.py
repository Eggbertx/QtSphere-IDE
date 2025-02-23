from PySide6.QtCore import QSize
from PySide6.QtGui import QUndoCommand

from commands.commandids import CommandIDs
from formats.spheremap import SphereMap
from widgets.map.mapview import MapView

class ResizeAllMapLayersCommand(QUndoCommand):
	mapView:MapView
	oldSizes:list[QSize]
	newSizes:list[QSize]

	@property
	def map(self) -> SphereMap:
		return self.mapView.mapFile

	def __init__(self, mapView:MapView, newSize:QSize):
		super().__init__()
		self.mapView = mapView
		self.oldSizes = [QSize(layer.width, layer.height) for layer in self.map.layers]
		self.newSizes = [newSize for _ in self.map.layers]


	def id(self) -> int:
		return CommandIDs.ResizeAllMapLayers.value


	def undo(self):
		for i in range(len(self.map.layers)):
			self.map.layers[i].resize(self.oldSizes[i].width(), self.oldSizes[i].height())
		self.mapView.attachTileset(self.map.tileset)


	def redo(self):
		for i in range(len(self.map.layers)):
			self.map.layers[i].resize(self.newSizes[i].width(), self.newSizes[i].height())
		self.mapView.attachTileset(self.map.tileset)
		

	def mergeWith(self, other:QUndoCommand) -> bool:
		return other.id() == self.id() and isinstance(other, ResizeAllMapLayersCommand) \
			and other.map.filePath == self.map.filePath \
			and other.newSizes == self.newSizes


class ResizeCurrentMapLayerCommand(QUndoCommand):
	mapView:MapView
	layerIndex:int
	oldSize:QSize
	newSize:QSize

	@property
	def map(self) -> SphereMap:
		return self.mapView.mapFile


	def __init__(self, mapView:MapView, layerIndex:int, newSize:QSize):
		super().__init__()
		self.mapView = mapView
		self.layerIndex = layerIndex
		self.oldSize = QSize(self.map.layers[layerIndex].width, self.map.layers[layerIndex].height)
		self.newSize = newSize


	def id(self) -> int:
		return CommandIDs.ResizeCurrentMapLayer.value


	def undo(self):
		self.map.layers[self.layerIndex].resize(self.oldSize.width(), self.oldSize.height())
		self.mapView.attachTileset(self.map.tileset)


	def redo(self):
		self.map.layers[self.layerIndex].resize(self.newSize.width(), self.newSize.height())
		self.mapView.attachTileset(self.map.tileset)


	def mergeWith(self, other:QUndoCommand) -> bool:
		return other.id() == self.id() and isinstance(other, ResizeCurrentMapLayerCommand) \
			and other.map.filePath == self.map.filePath \
			and other.layerIndex == self.layerIndex \
			and other.newSize == self.newSize