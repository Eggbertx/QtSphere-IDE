from enum import Enum, auto
from PySide6.QtCore import Qt, QSize, Signal, QPoint, QRect
from PySide6.QtWidgets import QGraphicsPixmapItem, QGraphicsScene, QGraphicsView, QWidget, QAbstractScrollArea
from PySide6.QtGui import QBrush, QColor, QImage, QMouseEvent, QPainter, QPen, QPixmap

from widgets.drawingenums import DrawingTool

class DrawingView(QGraphicsView):
	scene: QGraphicsScene
	drawingTool: DrawingTool
	image: QImage
	__modified: bool
	pressedButtons: int
	leftColor: QColor
	rightColor: QColor
	brushSize: int
	lastPos: QPoint
	imageItem: QGraphicsPixmapItem

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


	@staticmethod
	def openAndAttach(parent:QWidget, imagePath:str):
		image = QImage(imagePath)
		return DrawingView(parent, image)


	def __init__(self, parent: QWidget|None = None, image: QImage|None = None):
		super().__init__(parent)
		self.drawingTool = DrawingTool.Pencil
		self.__modified = False
		self.pressedButtons = 0
		self.setBackgroundBrush(QBrush(QPixmap(":/res/transparency-bg.png")))

		self.leftColor = QColor("black")
		self.rightColor = QColor("white")
		self.brushSize = 1
		self.lastPos = QPoint(-1, -1)
		self.scene = QGraphicsScene(self)
		self.setScene(self.scene)
		self.setVerticalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
		self.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)

		self.setAlignment(Qt.AlignmentFlag.AlignLeft | Qt.AlignmentFlag.AlignTop)
		self.attachImage(image)


	def setModified(self, modified:bool):
		# TODO: Either make this use QUndoStack, or have ImageEditor handle image editing
		oldModified = self.__modified
		self.__modified = modified
		if self.__modified != oldModified:
			self.modificationChanged.emit(modified)

	def scrolledPos(self, pos: QPoint) -> QPoint:
		return QPoint(
			pos.x() + self.horizontalScrollBar().value(),
			pos.y() + self.verticalScrollBar().value())


	def attachImage(self, image:QImage):
		if image is None:
			self.image = QImage(1, 1, QImage.Format.Format_RGBA8888)
			self.image.fill(Qt.GlobalColor.white)
		else:
			self.image = image

		self.scene.clear()
		self.imageItem = self.scene.addPixmap(QPixmap.fromImage(self.image))
		self.__updateSize()

	def zoomIn(self):
		self.image = self.image.scaled(self.image.width() * 2, self.image.height() * 2, Qt.AspectRatioMode.IgnoreAspectRatio, Qt.TransformationMode.FastTransformation)
		self.imageItem.setPixmap(QPixmap.fromImage(self.image))
		self.__updateSize()

	def zoomOut(self):
		self.image = self.image.scaled(self.image.width() // 2, self.image.height() // 2, Qt.AspectRatioMode.IgnoreAspectRatio, Qt.TransformationMode.FastTransformation)
		self.imageItem.setPixmap(QPixmap.fromImage(self.image))
		self.__updateSize()

	def __updateSize(self):
		self.setFixedSize(self.image.size())
		self.setSceneRect(0, 0, self.image.width(), self.image.height())
		self.viewport().update()

#region Event overloads
	def mousePressEvent(self, event: QMouseEvent) -> None:
		self.lastPos = self.scrolledPos(event.pos())
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
		scrolledPos = self.scrolledPos(event.pos())
		if self.imageRect.contains(scrolledPos) and (self.isLeftMouseBtnDown or self.isRightMouseBtnDown):
			painter = QPainter(self.image)
			
			painter.setPen(QPen(self.leftColor if self.isLeftMouseBtnDown else self.rightColor, self.brushSize,
				Qt.PenStyle.SolidLine, Qt.PenCapStyle.RoundCap, Qt.PenJoinStyle.RoundJoin))

			painter.drawLine(scrolledPos if self.lastPos.x() < 0 else self.lastPos, scrolledPos)
			self.imageItem.setPixmap(QPixmap.fromImage(self.image))
			self.update()
		self.lastPos = scrolledPos

#endregion
