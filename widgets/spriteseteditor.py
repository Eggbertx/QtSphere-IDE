from PySide6.QtCore import Qt, Slot
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import QWidget, QGraphicsPixmapItem, QGraphicsScene

from formats.spriteset import SphereSpriteset
from .sphereeditor import SphereEditor, SphereFile
from .imagechooser import ImageChooser

from ui.ui_spriteseteditor import Ui_SpritesetEditor

class SpritesetEditor(SphereEditor):
	ui: Ui_SpritesetEditor
	loadedSpriteset: SphereSpriteset
	imagesScene: QGraphicsScene
	directionScene: QGraphicsScene
	imageChooser: ImageChooser

	def __init__(self, parent: QWidget = None, windowType: Qt.WindowType = Qt.WindowType.Widget) -> None:
		super().__init__(parent, windowType)
		self.ui = Ui_SpritesetEditor()
		self.ui.setupUi(self)
		self.editorType = SphereFile.Spriteset
		self.loadedSpriteset = None
		self.directionScene = QGraphicsScene(self.ui.animView)
		self.ui.animView.setScene(self.directionScene)
		self.imagesScene = QGraphicsScene(self.ui.ssImages)
		self.ui.animDirChoose.currentIndexChanged.connect(self.onDirectionChanged)
		self.imageChooser = ImageChooser(self, True)
		self.ui.ssImages.setWidget(self.imageChooser)
		self.ui.ssImages.setAlignment(Qt.AlignmentFlag.AlignLeft|Qt.AlignmentFlag.AlignTop)
		self._adjustSizes()

	def _adjustSizes(self):
		self.ui.ssViewSplitter.setStretchFactor(0,4)
		self.ui.ssViewSplitter.setSizes((self.window().width()-164,164))
		self.ui.choosers.setSizes((200,150,200))

	def setDirection(self, d:int):
		self.directionScene.clear()
		image = self.loadedSpriteset.images[self.loadedSpriteset.directions[d].frames[0].imageIndex]
		item = QGraphicsPixmapItem(QPixmap.fromImage(image)) # the internal C++ object is deleted when scene.clear() is called so we have to recreate it
		item.setScale(2)
		self.directionScene.addItem(item)

	def attachSpriteset(self, spriteset:SphereSpriteset):
		self.ui.animDirChoose.clear()
		for direction in spriteset.directions:
			if len(direction.frames) > 0:
				self.ui.animDirChoose.addItem(direction.name)
		self.imageChooser.images = spriteset.images
		self.loadedSpriteset = spriteset
		self.setDirection(0)

	@Slot(int)
	def onDirectionChanged(self, d:int):
		if self.loadedSpriteset is None or d < 0 or d >= len(self.loadedSpriteset.directions):
			return
		self.setDirection(d)
		
