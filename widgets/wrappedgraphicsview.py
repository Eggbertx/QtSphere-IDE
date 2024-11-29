from PySide6.QtCore import Qt, QSize, QPoint, QRect, Signal
from PySide6.QtGui import QMouseEvent, QResizeEvent, QPixmap, QColor
from PySide6.QtWidgets import QWidget, QGraphicsView, QGraphicsScene, QGraphicsRectItem

from settings import Settings

class WrappedGraphicsView(QGraphicsView):
	selectedIndex:int
	wScene:QGraphicsScene
	__scaleMult:int
	pixmaps:list[QPixmap]
	selectionRect:QGraphicsRectItem
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


	@property
	def tSize(self) -> QSize:
		if len(self.pixmaps) == 0:
			return QSize(0,0)
		return self.pixmaps[0].size() * self.scaleFactor


	def __init__(self, parent: QWidget | None = None):
		super().__init__(parent)
		self.setHorizontalScrollBarPolicy(Qt .ScrollBarPolicy.ScrollBarAlwaysOff)
		self.setAlignment(Qt.AlignmentFlag.AlignTop|Qt.AlignmentFlag.AlignLeft)
		self.setBackgroundBrush(Qt.GlobalColor.darkGray)
		self.selectedIndex = 0
		self.wScene = QGraphicsScene(self)
		self.setScene(self.wScene)
		self.__scaleMult = 1
		self.pixmaps = []
		self.selectionRect = None


	def indexAt(self, pos:QPoint):
		x = 0
		y = 0
		for i in range(len(self.pixmaps)):
			if x + self.tSize.width() > self.width():
				x = 0
				y += self.tSize.height()
			if QRect(QPoint(x,y), self.tSize).contains(pos):
				return i
			x += self.tSize.width()
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
		self.arrangeItems(resetPixmaps=True)


	def insertPixmapAtSelected(self, pixmap:QPixmap):
		self.pixmaps.insert(self.selectedIndex, pixmap)
		self.arrangeItems(resetPixmaps=True)


	def addPixmap(self, pixmap:QPixmap):
		self.pixmaps.append(pixmap)
		self.arrangeItems(resetPixmaps=True)


	def removePixmap(self, index:int):
		self.pixmaps.pop(index)
		self.arrangeItems()


	def removeSelectedPixmap(self):
		self.removePixmap(self.selectedIndex)


	def resetPixmaps(self):
		x = 0
		y = 0
		self.wScene.clear()
		for p in range(len(self.pixmaps)):
			pixmap = self.pixmaps[p]
			item = self.wScene.addPixmap(QPixmap(pixmap))
			item.setScale(self.__scaleMult)
			if x + self.tSize.height() > self.width():
				x = 0
				y += self.tSize.height()

			x += self.tSize.width()


	def arrangeItems(self, width:int = -1, height:int = -1, resetPixmaps:bool = False):
		if len(self.pixmaps) == 0:
			return

		if resetPixmaps:
			self.resetPixmaps()

		settings = Settings()
		cursorColor = QColor(settings.mapCursorColor)
		
		if not cursorColor.isValid():
			cursorColor = QColor(0,128,255,128)
			settings.mapCursorColor = cursorColor
		cursorColor.setAlpha(128)

		x = 0
		y = 0
		if self.selectionRect is not None:
			self.wScene.removeItem(self.selectionRect)
		self.selectionRect = self.wScene.addRect(0, 0, self.pixmaps[0].width(), self.pixmaps[0].height())

		for i in range(len(self.wScene.items())):
			item = self.wScene.items()[i]
			if x + self.tSize.height() > self.width():
				x = 0
				y += self.tSize.height()
			item.setPos(x, y)
			item.setScale(self.scaleFactor)
			if not isinstance(item, QGraphicsRectItem):
				x += self.tSize.width()
			if self.selectedIndex == i:
				if x + self.tSize.width() < self.width():
					self.selectionRect.setPos(x, y)
				else:
					self.selectionRect.setPos(0, y + self.tSize.height())
				self.selectionRect.setBrush(cursorColor)
				self.selectionRect.setZValue(1)

		self.setSceneRect(0, 0, x + self.tSize.width(), y + self.tSize.height())
		print(self.sceneRect(), self.selectionRect.rect())
