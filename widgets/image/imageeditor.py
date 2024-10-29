from PySide6.QtGui import QAction, QImage
from PySide6.QtWidgets import  QMenu, QToolBar, QWidget
from widgets.sphereeditor import SphereEditor
from widgets.image.drawingview import DrawingView

class ImageEditor(SphereEditor):
	drawingView: DrawingView
	menuBar: QToolBar
	pencilMenu: QMenu

	@staticmethod
	def openAndAttach(parent: QWidget, filePath:str):
		img = QImage(filePath)
		return ImageEditor(parent, img)

	def __init__(self, parent: QWidget, image: QImage):
		super().__init__(parent)
		self.drawingView = DrawingView(self, image)