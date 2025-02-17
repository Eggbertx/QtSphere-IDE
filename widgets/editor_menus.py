from typing import Self

from PySide6.QtWidgets import QMenu, QWidget
from PySide6.QtGui import QAction

from dialogs.mappropertiesdialog import MapPropertiesDialog
from widgets.map.mapeditor import MapEditor
from widgets.spriteset.spriteseteditor import SpritesetEditor

class EditorMenuProvider:
	parent: QWidget

	def __init__(self, parent:QWidget = None):
		self.parent = parent

	def createMapMenu(self, editor:MapEditor):
		mapMenu = QMenu(self.parent)
		mapMenu.setTitle("Map")
		mapMenu.addAction("Properties", lambda: self.__showMapProperties(editor))
		mapMenu.addSeparator()
		mapTilesetMenu = mapMenu.addMenu("Tileset")
		mapMenu.addAction("Resize All Layers")
		mapMenu.addAction("Resize Current Layer")
		mapMenu.addSeparator()
		mapMenu.addAction("Export image")

		mapTilesetMenu.addAction("Change Tile Size")
		mapTilesetMenu.addAction("Rescale Tileset")
		mapTilesetMenu.addSeparator()
		mapTilesetMenu.addAction("Change Tileset")
		mapTilesetMenu.addAction("Import")
		mapTilesetMenu.addAction("Export")
		mapTilesetMenu.addAction("Prune")
		return mapMenu


	def __showMapProperties(self, editor:MapEditor):
		mapPropertiesDialog = MapPropertiesDialog(editor, editor.map)
		if mapPropertiesDialog.exec() != 0:
			editor.mapPropertiesChanged.emit(mapPropertiesDialog)


	def createSpritesetMenu(self, editor:SpritesetEditor):
		spritesetMenu = QMenu(self.parent)
		spritesetMenu.setTitle("Spriteset")
		zoomMenu = spritesetMenu.addMenu("Zoom")
		zoomMenu.addAction("1x")
		zoomMenu.addAction("2x")
		zoomMenu.addAction("4x")
		zoomMenu.addAction("8x")
		spritesetMenu.addSeparator()
		sizeMenu = spritesetMenu.addMenu("Change Size")
		sizeMenu.addAction("Change Dimensions")
		sizeMenu.addAction("Rescale Spriteset")
		spritesetMenu.addAction("Set Delay (all frames)")
		spritesetMenu.addAction("Frame Properties")
		spritesetMenu.addAction("Export Frame as Image")
		spritesetMenu.addAction("Export Spriteset as Image")
		return spritesetMenu