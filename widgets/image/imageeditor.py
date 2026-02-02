from PySide6.QtCore import Qt
from PySide6.QtGui import QImage
from PySide6.QtWidgets import  QMenu, QWidget, QVBoxLayout

from ui.ui_imageeditor import Ui_ImageEditor

from widgets.sphereeditor import SphereEditor, SphereEditorType
from widgets.image.drawingtoolbar import DrawingToolbar

class ImageEditor(SphereEditor):
	ui: Ui_ImageEditor
	menuBar: DrawingToolbar
	pencilMenu: QMenu
	viewLayout: QVBoxLayout

	@staticmethod
	def openAndAttach(parent: QWidget, filePath:str):
		img = QImage(filePath)
		return ImageEditor(parent, img)

	def __init__(self, parent: QWidget|None = None, image: QImage = None):
		super().__init__(parent, SphereEditorType.Image)
		self.ui = Ui_ImageEditor()
		self.ui.setupUi(self)
		self.attachImage(image)

		self.menuBar = DrawingToolbar(self)
		self.menuBar.zoomInAction.triggered.connect(self.onZoomInClicked)
		self.menuBar.zoomOutAction.triggered.connect(self.onZoomOutClicked)
		self.ui.scrollAreaLayout.setMenuBar(self.menuBar)
		self.ui.scrollAreaLayout.setAlignment(Qt.AlignmentFlag.AlignLeft|Qt.AlignmentFlag.AlignTop)

	def onZoomInClicked(self):
		self.ui.drawingView.zoomIn()

	def onZoomOutClicked(self):
		self.ui.drawingView.zoomOut()

	def attachImage(self, image:QImage):
		self.ui.drawingView.attachImage(image)