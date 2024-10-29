from enum import Enum, auto
from PySide6.QtCore import Qt, QSize, Signal, QPoint, QRect
from PySide6.QtWidgets import QWidget
from PySide6.QtGui import QMouseEvent, QPaintEvent, QImage, QColor, QPainter, QPen


class DrawingMode(Enum):
	Pencil = auto()
	Line = auto()
	Rectangle = auto()
	Fill = auto()

class DrawingView(QWidget):
	drawingMode: DrawingMode
	image: QImage
	__modified: bool
	pressedButtons: int
	leftColor: QColor
	rightColor: QColor
	brushSize: int
	lastPos: QPoint

	@property
	def isLeftMouseBtnDown(self) -> bool:
		# left mouse button is currently pressed
		return self.pressedButtons & 1 == 1

	@property
	def isRightMouseBtnDown(self) -> bool:
		# right mouse button is currently pressed
		return self.pressedButtons & 2 == 2

	@property
	def imageSize(self) -> QSize:
		return self.image.size()

	@property
	def imageRect(self) -> QRect:
		return self.image.rect()

	modificationChanged:Signal = Signal(bool)

	def __init__(self, parent: QWidget|None = None, image: QImage|None = None):
		super().__init__(parent)
		self.drawingMode = DrawingMode.Pencil
		self.__modified = False
		self.pressedButtons = 0
		if image is None:
			self.image = QImage(320, 240, QImage.Format.Format_RGBA8888)
			self.image.fill(Qt.GlobalColor.white)
		else:
			self.image = image
		self.leftColor = QColor("black")
		self.rightColor = QColor("white")
		self.brushSize = 1
		self.lastPos = QPoint(-1, -1)


	def setModified(self, modified:bool):
		oldModified = self.__modified
		self.__modified = modified
		if self.__modified != oldModified:
			self.modificationChanged.emit(modified)


	def isOutOfBounds(self, pos:QPoint) -> bool:
		return pos.x() < 0 or pos.y() < 0 or pos.x() > self.image.width() or pos.y() > self.image.height()

#region Event overloads
	def mousePressEvent(self, event: QMouseEvent) -> None:
		self.lastPos = event.pos()
		match event.button():
			case Qt.MouseButton.LeftButton:
				self.pressedButtons |= 1
			case Qt.MouseButton.RightButton:
				self.pressedButtons |= 2


	def mouseReleaseEvent(self, event: QMouseEvent) -> None:
		self.lastPos = QPoint(-1,-1)
		match event.button():
			case Qt.MouseButton.LeftButton:
				self.pressedButtons &= 0xFE
			case Qt.MouseButton.RightButton:
				self.pressedButtons &= 0xFD


	def mouseMoveEvent(self, event: QMouseEvent) -> None:
		pos = event.pos()
		if self.isOutOfBounds(pos):
			return
		if self.isLeftMouseBtnDown or self.isRightMouseBtnDown:
			painter = QPainter(self.image)
			
			painter.setPen(QPen(self.leftColor if self.isLeftMouseBtnDown else self.rightColor, self.brushSize,
				Qt.PenStyle.SolidLine, Qt.PenCapStyle.RoundCap, Qt.PenJoinStyle.RoundJoin))

			painter.drawLine(event.pos() if self.lastPos.x() < 0 else self.lastPos, event.pos())
			self.lastPos = pos
			self.update()


	def paintEvent(self, event: QPaintEvent) -> None:
		canvasPainter = QPainter(self)
		canvasPainter.drawImage(self.image.rect(), self.image, self.image.rect())

#endregion

	@staticmethod
	def openAndAttach(parent:QWidget, imagePath:str):
		image = QImage()
		image.load(imagePath)
		return DrawingView(parent, image)
