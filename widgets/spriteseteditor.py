from PySide6.QtCore import Qt, Slot
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import QWidget, QGraphicsPixmapItem, QGraphicsScene

from formats.spriteset import SphereSpriteset
from .sphereeditor import SphereEditor, SphereFile

from ui.ui_spriteseteditor import Ui_SpritesetEditor

class SpritesetEditor(SphereEditor):
	ui: Ui_SpritesetEditor
	loadedSpriteset: SphereSpriteset
	directionItems: list[QGraphicsPixmapItem]
	def __init__(self, parent: QWidget = None, windowType: Qt.WindowType = Qt.WindowType.Widget) -> None:
		super().__init__(parent, windowType)
		self.ui = Ui_SpritesetEditor()
		self.ui.setupUi(self)
		self.editorType = SphereFile.Spriteset
		self.loadedSpriteset = None
		self.ui.animView.setScene(QGraphicsScene(self.ui.animView))
		self.ui.animDirChoose.currentIndexChanged.connect(self.onDirectionChanged)
		self.directionItems = []
		self._adjustSizes()

	def _adjustSizes(self):
		self.ui.ssViewSplitter.setStretchFactor(0,4)
		self.ui.ssViewSplitter.setSizes((self.window().width()-164,164))
		self.ui.choosers.setSizes((200,150,200))

	def setDirection(self, d:int):
		self.ui.animView.scene().clear()
		self.ui.animView.scene().addItem(self.directionItems[d])

	def attachSpriteset(self, spriteset:SphereSpriteset):
		self.ui.animDirChoose.clear()
		for direction in spriteset.directions:
			if len(direction.frames) > 0:
				imageIndex = direction.frames[0].imageIndex
				item = QGraphicsPixmapItem(QPixmap.fromImage(spriteset.images[imageIndex]))
				item.setScale(2)
				self.directionItems.append(item)
				self.ui.animDirChoose.addItem(direction.name)
		self.setDirection(0)
		self.loadedSpriteset = spriteset

	@Slot(int)
	def onDirectionChanged(self, d:int):
		if self.loadedSpriteset is None or d < 0 or d >= len(self.loadedSpriteset.directions):
			return
		self.setDirection(d)
		
