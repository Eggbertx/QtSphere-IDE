from enum import Enum, auto
from PySide6.QtCore import Qt, QSize, Signal, QPoint, QRectF
from PySide6.QtWidgets import QGraphicsView, QGraphicsScene, QWidget, QGraphicsPixmapItem
from PySide6.QtGui import QMouseEvent, QPixmap, QImage, QColor


class DrawingMode(Enum):
	Pencil = auto()
	Line = auto()
	Rectangle = auto()
	Fill = auto()

class DrawingView(QGraphicsView):
	drawingMode: DrawingMode
	image: QImage
	pixmap: QPixmap
	pixmapItem: QGraphicsPixmapItem
	__modified:bool
	pressedButtons:int
	currentColor: QColor

	@property
	def isDrawing(self) -> bool:
		# left mouse button is currently pressed
		return self.pressedButtons & 1 == 1

	@property
	def isErasing(self) -> bool:
		# right mouse button is currently pressed
		return self.pressedButtons & 2 == 2

	modificationChanged:Signal = Signal(bool)
	def __init__(self, parent: QWidget|None = None, image: QImage|None = None):
		super().__init__(parent)
		self.setScene(QGraphicsScene(self))
		self.setAlignment(Qt.AlignmentFlag.AlignLeft|Qt.AlignmentFlag.AlignTop)
		self.drawingMode = DrawingMode.Pencil
		# self.erasing = False
		self.__modified = False
		self.pressedButtons = 0
		if image is None:
			self.image = QImage(320, 240, QImage.Format.Format_RGBA8888)
		else:
			self.image = image
		self.pixmap = QPixmap(self.image)
		self.pixmapItem = self.scene().addPixmap(self.pixmap)
		self.currentColor = QColor("red")

	def imageSize(self) -> QSize:
		return self.image.size()

	def setModified(self, modified:bool):
		oldModified = self.__modified
		self.__modified = modified
		if self.__modified != oldModified:
			self.modificationChanged.emit(modified)

	def __doDrawing(self, pos:QPoint):
		if pos.x() < 0 or pos.y() < 0 or pos.x() > self.image.width() or pos.y() > self.image.height():
			return
		print("Drawing at position ", pos.x(), pos.y())


#region Event overloads
	def mousePressEvent(self, event: QMouseEvent) -> None:
		match event.button():
			case Qt.MouseButton.LeftButton:
				self.pressedButtons |= 1
			case Qt.MouseButton.RightButton:
				self.pressedButtons |= 2
		return super().mousePressEvent(event)

	def mouseReleaseEvent(self, event: QMouseEvent) -> None:
		match event.button():
			case Qt.MouseButton.LeftButton:
				self.pressedButtons &= 0xFE
			case Qt.MouseButton.RightButton:
				self.pressedButtons &= 0xFD
		return super().mouseReleaseEvent(event)

	def mouseMoveEvent(self, event: QMouseEvent) -> None:
		pos = event.pos()
		if self.isDrawing:
			self.__doDrawing(pos)
		elif self.isErasing:
			pass
		return super().mouseMoveEvent(event)
#endregion


	@staticmethod
	def openAndAttach(parent:QWidget, imagePath:str):
		image = QImage()
		image.load(imagePath)
		return DrawingView(parent, image)
