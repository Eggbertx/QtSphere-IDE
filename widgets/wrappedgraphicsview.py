from PySide6.QtCore import Qt, QSize, QPoint, QRect, Signal
from PySide6.QtGui import QMouseEvent, QResizeEvent, QPixmap, QColor
from PySide6.QtWidgets import QWidget, QGraphicsView, QGraphicsScene

from settings import Settings

class WrappedGraphicsView(QGraphicsView):
	selectedIndex:int
	tSize:QSize
	wScene:QGraphicsScene
	__scaleMult:int
	pixmaps:list[QPixmap]
	indexChanged:Signal = Signal(int)

	@property
	def scaleFactor(self):
		return self.__scaleMult
	
	@scaleFactor.setter
	def scaleFactor(self, factor:int):
		if factor < 1:
			return
		if self.__scaleMult != factor:
			self.__scaleMult = factor
			self.arrangeItems(self.width(), self.height())


	def __init__(self, parent: QWidget | None = None):
		super().__init__(parent)
		self.setHorizontalScrollBarPolicy(Qt .ScrollBarPolicy.ScrollBarAlwaysOff)
		self.setAlignment(Qt.AlignmentFlag.AlignTop|Qt.AlignmentFlag.AlignLeft)
		self.setBackgroundBrush(Qt.GlobalColor.darkGray)
		self.selectedIndex = 0
		self.wScene = QGraphicsScene(self)
		self.setScene(self.wScene)
		self.tSize = QSize(0,0)
		self.__scaleMult = 1
		self.pixmaps = []


	def indexAt(self, pos:QPoint):
		x = 0
		y = 0
		for i in range(len(self.pixmaps)):
			size = self.pixmaps[i].size()
			if x + size.width() > self.width():
				x = 0
				y += size.height()
			if QRect(QPoint(x,y), size).contains(pos):
				return i
			x += size.width()
		return -1


	def resizeEvent(self, event: QResizeEvent):
		self.arrangeItems(event.size().width(), event.size().height())
		event.accept()


	def applyClick(self, pos:QPoint):
		newIndex = self.indexAt(pos)
		if newIndex > -1:
			if self.selectedIndex != newIndex:
				self.indexChanged.emit(newIndex)
			self.selectedIndex = newIndex
			self.arrangeItems(self.width(), self.height())


	def mouseReleaseEvent(self, event: QMouseEvent):
		self.applyClick(event.pos())


	def insertPixmap(self, pixmap:QPixmap, index:int):
		self.pixmaps.insert(index, pixmap)
		self.arrangeItems()


	def insertPixmapAtSelected(self, pixmap:QPixmap):
		self.pixmaps.insert(self.selectedIndex, pixmap)
		self.arrangeItems()


	def addPixmap(self, pixmap:QPixmap):
		self.pixmaps.append(pixmap)
		self.arrangeItems()


	def removePixmap(self, index:int):
		self.pixmaps.pop(index)
		self.arrangeItems()


	def removeSelectedPixmap(self):
		self.removePixmap(self.selectedIndex)


	def setZoom(self, zoom:int):
		self.__scaleMult = zoom
		self.arrangeItems()


	def arrangeItems(self, width:int = -1, height:int = -1):
		if len(self.pixmaps) == 0:
			return

		settings = Settings()
		cursorColor = QColor(settings.mapCursorColor)
		if not cursorColor.isValid():
			cursorColor = QColor(0,128,255,128)
			settings.mapCursorColor = cursorColor

		cursorColor.setAlpha(128)
		x = 0
		y = 0
		rows = 0

		if width == -1 and height == -1:
			self.tSize = self.pixmaps[0].size()
		else:
			self.tSize = self.wScene.items()[0].boundingRect().size().toSize() 

		self.wScene.clear()
		for p in range(len(self.pixmaps)):
			pixmap = self.pixmaps[p]
			item = self.wScene.addPixmap(QPixmap(pixmap))
			item.setScale(self.__scaleMult)
			if x + self.tSize.height() > self.width():
				x = 0
				y += self.tSize.height()

			item.setPos(x * self.scaleFactor, y * self.scaleFactor)
			if self.selectedIndex == p:
				selectionBox = self.wScene.addRect(x, y, self.tSize.width() - 1, self.tSize.height() - 1)
				selectionBox.setBrush(cursorColor)
				selectionBox.setZValue(1)
			
			self.setSceneRect(0, 0, x + self.tSize.width() * self.scaleFactor, y + self.tSize.height() * self.scaleFactor)
			x += self.tSize.width()
		
		return rows
