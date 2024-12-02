from PySide6.QtCore import Qt
from PySide6.QtWidgets import QDialog, QWidget, QGraphicsView

from ui.ui_tilepropertiesdialog import Ui_TilePropertiesDialog

from formats.spheremap import Tile
from widgets.image.drawingview import DrawingView

class TilePropertiesDialog(QDialog):
	ui: Ui_TilePropertiesDialog
	tile: Tile
	tileImageEditView: DrawingView
	tileObstructionEditView: DrawingView
	tileAnimationPreviewView: QGraphicsView

	def __init__(self, parent: QWidget | None = None):
		super().__init__(parent)
		self.ui = Ui_TilePropertiesDialog()
		self.ui.setupUi(self)


	def show(self, tile:Tile) -> None:
		self.tile = tile
		self.ui.image_tab.attachImage(self.tile.image.copy(self.tile.image.rect()))
		self.ui.tabWidget.setCurrentIndex(0)
		return super().show()