from PySide6.QtCore import Qt
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import QWidget, QGraphicsPixmapItem, QGraphicsScene

from formats.spriteset import SphereSpriteset
from ui.ui_spriteseteditor import Ui_SpritesetEditor

class SpritesetEditor(QWidget):
	ui: Ui_SpritesetEditor
	loadedSpriteset: SphereSpriteset
	def __init__(self, parent: QWidget = None, windowType: Qt.WindowType = Qt.WindowType.Widget) -> None:
		super().__init__(parent, windowType)
		self.ui = Ui_SpritesetEditor()
		self.ui.setupUi(self)
		self.loadedSpriteset = None
		self.ui.animView.setScene(QGraphicsScene(self.ui.animView))
		self._adjustSizes()

	def _adjustSizes(self):
		self.ui.ssViewSplitter.setStretchFactor(0,4)
		self.ui.ssViewSplitter.setSizes((self.window().width()-164,164))
		self.ui.choosers.setSizes((200,150,200))
	
	def attachSpriteset(self, spriteset:SphereSpriteset):
		item = QGraphicsPixmapItem(QPixmap.fromImage(spriteset.images[0]))
		self.ui.animView.scene().addItem(item)
		self.ui.animDirChoose.clear()
		for dir in spriteset.directions:
			self.ui.animDirChoose.addItem(dir.name)
		self.loadedSpriteset = spriteset
