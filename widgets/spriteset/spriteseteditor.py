from PySide6.QtCore import Qt, Slot, QObject
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import QWidget, QGraphicsPixmapItem, QGraphicsScene

from formats.spriteset import SphereSpriteset
from ..sphereeditor import SphereEditor, SphereFile
from .imagechooser import ImageChooser
from .directionview import DirectionView

from ui.ui_spriteseteditor import Ui_SpritesetEditor

class SpritesetEditor(SphereEditor):
	ui: Ui_SpritesetEditor
	loadedSpriteset: SphereSpriteset
	imagesScene: QGraphicsScene
	directionScene: QGraphicsScene
	directionViews: list[DirectionView]
	imageChooser: ImageChooser

	@staticmethod
	def openAndAttach(parent: QWidget, filePath:str):
		rss = SphereSpriteset(filePath)
		rss.open()
		editor = SpritesetEditor(parent)
		editor.attachSpriteset(rss)
		return editor

	def __init__(self, parent: QWidget = None):
		super().__init__(parent)
		self.ui = Ui_SpritesetEditor()
		self.ui.setupUi(self)
		self.directionViews = []
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
		self.filePath = spriteset.filePath
		for d in range(len(spriteset.directions)):
			self.ui.animDirChoose.addItem(spriteset.directions[d].name)
			view = DirectionView(self, spriteset, d, self.undoStack)
			# view.addFrameButton.clicked.connect(lambda: self.onAddDirectionFrameClicked(d))
			self.directionViews.append(view)
			self.ui.dirsContainer.addWidget(view)


		self.imageChooser.images = spriteset.images
		self.loadedSpriteset = spriteset
		self.setDirection(0)

	@Slot(int)
	def onDirectionChanged(self, d:int):
		if self.loadedSpriteset is None or d < 0 or d >= len(self.loadedSpriteset.directions):
			return
		self.setDirection(d)
