from PySide6.QtCore import Qt, QEvent, QObject, QRect, QPoint
from PySide6.QtGui import QPainter, QBrush
from PySide6.QtWidgets import QWidget, QGraphicsPixmapItem, QGraphicsView, QGraphicsScene
from PySide6.QtGui import QPixmap, QImage

from ui.ui_windowstylepreview import Ui_WindowStylePreview

from formats.windowstyle import SphereWindowStyle, WindowStyleBitmap


class WindowStylePreview(QWidget):
	ui: Ui_WindowStylePreview

	@property
	def ulGraphics(self):
		return self.ui.ulGraphics
	
	@property
	def topGraphics(self):
		return self.ui.topGraphics
	
	@property
	def urGraphics(self):
		return self.ui.urGraphics
	
	@property
	def leftGraphics(self):
		return self.ui.leftGraphics
	
	@property
	def bgGraphics(self):
		return self.ui.bgGraphics
	
	@property
	def rightGraphics(self):
		return self.ui.rightGraphics
	
	@property
	def llGraphics(self):
		return self.ui.llGraphics
	
	@property
	def bottomGraphics(self):
		return self.ui.bottomGraphics
	
	@property
	def lrGraphics(self):
		return self.ui.lrGraphics

	def __init__(self, parent,):
		super().__init__(parent)
		self.ui = Ui_WindowStylePreview()
		self.ui.setupUi(self)

		gfxViews = (
			self.ulGraphics, self.topGraphics, self.urGraphics,
			self.leftGraphics, self.bgGraphics, self.rightGraphics,
			self.llGraphics, self.bottomGraphics, self.lrGraphics
		)
		self.alphaBG = QPixmap(":/res/transparency-bg.png")
		for view in gfxViews:
			view.setBackgroundBrush(QBrush(self.alphaBG))
			scene = view.scene()
			if scene is None:
				scene = QGraphicsScene()
				view.setScene(scene)
			scene.setBackgroundBrush(QBrush(self.alphaBG))
		self.installEventFilter(self)

	def getView(self, target: WindowStyleBitmap) -> QGraphicsView:
		match target:
			case WindowStyleBitmap.UpperLeft:
				return self.ulGraphics
			case WindowStyleBitmap.Top:
				return self.topGraphics
			case WindowStyleBitmap.UpperRight:
				return self.urGraphics
			case WindowStyleBitmap.Left:
				return self.leftGraphics
			case WindowStyleBitmap.Background:
				return self.bgGraphics
			case WindowStyleBitmap.Right:
				return self.rightGraphics
			case WindowStyleBitmap.LowerLeft:
				return self.llGraphics
			case WindowStyleBitmap.Bottom:
				return self.bottomGraphics
			case WindowStyleBitmap.LowerRight:
				return self.lrGraphics
		raise ValueError("Invalid WindowStyleBitmap value")

	def setBitmap(self, target: WindowStyleBitmap, bmp: QImage):
		view = self.getView(target)
		if target in (WindowStyleBitmap.UpperLeft, WindowStyleBitmap.UpperRight,
					  WindowStyleBitmap.LowerLeft, WindowStyleBitmap.LowerRight):
			view.setFixedSize(bmp.width(), bmp.height())
		scene = view.scene()
		scene.clear()
		pixmap = QPixmap.fromImage(bmp)
		item = QGraphicsPixmapItem(pixmap)
		scene.addItem(item)
	

	def eventFilter(self, watched: QObject, event: QEvent) -> bool:
		return True