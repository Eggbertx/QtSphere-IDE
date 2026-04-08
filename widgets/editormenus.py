from math import pow
from os import path

from PySide6.QtCore import QSize
from PySide6.QtWidgets import QMenu, QWidget, QFileDialog, QMessageBox
from PySide6.QtGui import QAction

from dialogs.mappropertiesdialog import MapPropertiesDialog
from dialogs.resizedialog import ResizeDialog
from dialogs.windowstylepropertiesdialog import WindowStylePropertiesDialog
from formats.tileset import Tileset
from widgets.map.mapeditor import MapEditor
from widgets.spriteset.spriteseteditor import SpritesetEditor
from widgets.windowstyle.windowstyleeditor import WindowStyleEditor

class EditorMenuProvider:
	parent: QWidget

	def __init__(self, parent:QWidget = None):
		self.parent = parent


	def setCheckedAction(self, menu: QMenu, a:QAction):
		items = menu.actions()
		for item in items:
			item.setCheckable(True)
			item.setChecked(item == a)


	def __showResizeAllLayersDialog(self, editor:MapEditor):
		resizeDialog = ResizeDialog("Resize All Layers", QSize(editor.map.layers[0].width, editor.map.layers[0].height), editor)
		resizeDialog.suffix = " tiles"
		if resizeDialog.exec() != 0:
			editor.allLayersResized.emit(resizeDialog.sizeValue)


	def __showResizeCurrentLayerDialog(self, editor:MapEditor):
		currentLayer = editor.ui.layersTable.currentLayer()
		resizeDialog = ResizeDialog("Resize Current Layer", QSize(editor.map.layers[currentLayer].width, editor.map.layers[currentLayer].height), editor)
		resizeDialog.suffix = " tiles"
		if resizeDialog.exec() != 0:
			editor.currentLayerResized.emit(currentLayer, resizeDialog.sizeValue)


	def __showChangeTileSizeDialog(self, editor:MapEditor):
		resizeDialog = ResizeDialog("Change Tile Size", QSize(editor.map.tileset.tileWidth, editor.map.tileset.tileHeight), editor)
		resizeDialog.suffix = " px"
		if resizeDialog.exec() != 0:
			editor.tileSizeChanged.emit(resizeDialog.sizeValue)


	def __showRescaleTilesetDialog(self, editor:MapEditor):
		resizeDialog = ResizeDialog("Rescale Tileset", QSize(editor.map.tileset.tileWidth, editor.map.tileset.tileHeight), editor)
		resizeDialog.suffix = " px"
		if resizeDialog.exec() != 0:
			editor.tilesetRescaled.emit(resizeDialog.sizeValue)


	def __showReplaceTilesetDialog(self, editor:MapEditor):
		fileDialog = QFileDialog(self.parent, "Replace Tileset", path.dirname(editor.map.filePath), "Sphere Tileset (*.rts);;All Files (*)")
		fileDialog.setAcceptMode(QFileDialog.AcceptMode.AcceptOpen)
		fileDialog.setFileMode(QFileDialog.FileMode.ExistingFile)
		if fileDialog.exec() != 0:
			replacementRts = Tileset(fileDialog.selectedFiles()[0])
			replacementRts.open()
			if len(replacementRts.tiles) < len(editor.map.tileset.tiles):
				QMessageBox.critical(self.parent, "Unable To Replace Tileset",
					"The replacement tileset has fewer tiles than the current tileset.", QMessageBox.StandardButton.Ok)
				return
			editor.tilesetReplaced.emit(replacementRts)


	def createMapMenu(self, editor:MapEditor):
		mapMenu = QMenu(self.parent)
		mapMenu.setTitle("Map")
		mapMenu.addAction("Properties", lambda: self.__showMapProperties(editor))
		mapMenu.addSeparator()
		mapTilesetMenu = mapMenu.addMenu("Tileset")
		mapMenu.addAction("Resize All Layers", lambda: self.__showResizeAllLayersDialog(editor))
		mapMenu.addAction("Resize Current Layer", lambda: self.__showResizeCurrentLayerDialog(editor))
		mapMenu.addSeparator()
		mapMenu.addAction("Export image")

		mapTilesetMenu.addAction("Change Tile Size", lambda: self.__showChangeTileSizeDialog(editor))
		mapTilesetMenu.addAction("Rescale Tileset", lambda: self.__showRescaleTilesetDialog(editor))
		mapTilesetMenu.addSeparator()
		mapTilesetMenu.addAction("Replace Tileset", lambda: self.__showReplaceTilesetDialog(editor))
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


	def __showWindowStylePropertiesDialog(self, editor:WindowStyleEditor):
		dialog = WindowStylePropertiesDialog(editor, editor.windowStyle)
		if dialog.exec() != 0:
			editor.windowStylePropertiesChanged.emit(dialog)


	def createWindowStyleMenu(self, editor:WindowStyleEditor):
		windowStyleMenu = QMenu(self.parent)
		windowStyleMenu.setTitle("Window Style")
		editMenu = windowStyleMenu.addMenu("Edit")
		
		bitmaps = ("Upper Left", "Top", "Upper Right", "Right", "Lower Right", "Bottom", "Lower Left", "Left", "Background")
		for b in range(len(bitmaps)):
			action = editMenu.addAction(bitmaps[b])
			action.setCheckable(True)
			action.triggered.connect(lambda: editor.preview.setActiveBitmap(b))
			action.triggered.connect(lambda: self.setCheckedAction(editMenu, editMenu.actions()[b]))
			action.setData(b)
		editMenu.actions()[0].setChecked(True)

		zoomMenu = windowStyleMenu.addMenu("Zoom")
		for i in range(3):
			action = zoomMenu.addAction(f"{int(pow(2, i))}x")
			action.setCheckable(True)
			
		zoomMenu.actions()[1].setChecked(True)
		zoomMenu.triggered.connect(editor.zoomMenuTriggered)

		sizeMenu = windowStyleMenu.addMenu("Change Size")
		sizeMenu.addAction("Change Dimensions")
		sizeMenu.addAction("Rescale Current Bitmap")

		windowStyleMenu.addSeparator()
		windowStyleMenu.addAction("Properties", lambda: self.__showWindowStylePropertiesDialog(editor))
		return windowStyleMenu
