from PySide6.QtCore import QEvent, QObject, Qt, QPoint, Signal, QRect
from PySide6.QtGui import QPixmap, QImage, QPalette, QPainter, QMouseEvent
from PySide6.QtWidgets import QWidget, QMenu

class ImageChooser(QWidget):
	showTransparency:bool
	alphaBG: QPixmap
	_images: list[QImage]
	selectedIndex: int
	mousePos: QPoint
	contextMenu: QMenu
	imageChosen: Signal = Signal(int)

	@property
	def images(self):
		return self._images

	@images.setter
	def images(self, replacing: list[QImage]):
		self._images = replacing
		self.repaint()

	@images.deleter
	def images(self):
		del self._images


	def __init__(self, parent: QWidget | None = None, showTransparency:bool = True):
		super().__init__(parent)
		self.installEventFilter(self)
		self.showTransparency = showTransparency
		self.alphaBG = QPixmap.fromImage(QImage(":/icons/res/transparency-bg.png"))

		self.images = []
		self.selectedIndex = 0
		
		pal = self.palette()
		pal.setColor(QPalette.ColorRole.Window, Qt.GlobalColor.black)
		self.setAutoFillBackground(True)
		self.setPalette(pal)

		self.contextMenu = QMenu(self)
		fileMenu = self.contextMenu.addMenu("File")
		fileMenu.addAction("Save frame")
		fileMenu.addAction("Replace frame")
		fileMenu.addAction("Append from file")
		self.contextMenu.addAction("Append")
		self.contextMenu.addAction("Remove")
		self.contextMenu.addAction("Remove #")
	
	def eventFilter(self, watched: QObject, event: QEvent) -> bool:
		match event.type():
			case QEvent.Type.Paint:
				painter = QPainter(self)
				painter.drawTiledPixmap(self.rect(), self.alphaBG)
				x = 0
				y = 0
				for i in range(len(self.images)):
					image = self.images[i]
					if x + image.width() > self.width():
						x = 0
						y += image.height()
					painter.drawImage(QPoint(x,y), image)
					if i == self.selectedIndex:
						painter.setPen(Qt.GlobalColor.magenta)
						painter.drawRect(x, y, image.width(), image.height())
					x += image.width()

			case QEvent.Type.MouseMove:
				self.mousePos = event.pos()

			case QEvent.Type.MouseButtonPress:
				pressed:int = event.button()
				pos:QPoint = event.pos()
				if pressed == Qt.MouseButton.LeftButton or pressed == Qt.MouseButton.RightButton:
					x = 0
					y = 0
					for i in range(len(self.images)):
						image = self.images[i]
						if x + image.width() > self.width():
							x = 0
							y += image.height()
						if x <= pos.x() and pos.x() < x + image.width() and y <= pos.y() and pos.y() < y + image.height():
							self.selectedIndex = i
							self.imageChosen.emit(i)
							self.repaint()
						x += image.width()

				if pressed == Qt.MouseButton.RightButton:
					self.contextMenu.exec(self.mapToGlobal(pos))

		return True