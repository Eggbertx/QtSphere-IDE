from PySide6.QtGui import QAction, QImage
from PySide6.QtWidgets import  QMenu, QToolBar, QWidget, QVBoxLayout
from widgets.sphereeditor import SphereEditor
from widgets.image.drawingview import DrawingView
from widgets.image.drawingtoolbar import DrawingToolbar

class ImageEditor(SphereEditor):
	drawingView: DrawingView
	menuBar: DrawingToolbar
	pencilMenu: QMenu
	viewLayout: QVBoxLayout

	@staticmethod
	def openAndAttach(parent: QWidget, filePath:str):
		img = QImage(filePath)
		return ImageEditor(parent, img)

	def __init__(self, parent: QWidget|None = None, image: QImage = None):
		super().__init__(parent)
		self.drawingView = DrawingView(self, image)
		self.menuBar = DrawingToolbar(self)
		self.viewLayout = QVBoxLayout()
		self.viewLayout = QVBoxLayout(self)
		self.viewLayout.setSpacing(0)
		self.viewLayout.setContentsMargins(0, 0, 0, 0)
		self.viewLayout.addWidget(self.drawingView)
		self.viewLayout.addStretch(0)
		self.setLayout(self.viewLayout)
		self.viewLayout.setMenuBar(self.menuBar)


	def attachImage(self, image:QImage):
		self.drawingView.attachImage(image)