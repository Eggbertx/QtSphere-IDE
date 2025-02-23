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
		tilesPerRow = self.width() // self.tSize.width()
		px = pos.x() // self.tSize.width()
		py = pos.y() // self.tSize.height()
		i = px + tilesPerRow * py
		if i >= len(self.pixmaps):
			return -1
		return i


	def indexToPosition(self, index:int):
		# returns QPoint given an index (doesn't do bounds checking)
		tw = self.tSize.width()
		tilesPerRow = max(self.width() // self.tSize.width(),1)
		return QPoint((index % tilesPerRow) * tw, (index // tilesPerRow) * tw)


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
		self.wScene.clear()
		self.selectionRect = None
		for p in range(len(self.pixmaps)):
			pixmap = self.pixmaps[p]
			item = self.wScene.addPixmap(QPixmap(pixmap))
			item.setScale(self.__scaleMult)



	def arrangeItems(self, width:int = -1, height:int = -1, resetPixmaps:bool = False):
		if len(self.pixmaps) == 0 or self.width() < 1:
			return

		if resetPixmaps:
			self.resetPixmaps()

		settings = Settings()
		cursorColor = QColor(settings.mapCursorColor)
		
		if not cursorColor.isValid():
			cursorColor = QColor(0,128,255,128)
			settings.mapCursorColor = cursorColor
		cursorColor.setAlpha(128)

		if self.selectionRect is not None:
			self.wScene.removeItem(self.selectionRect)
		self.selectionRect = self.wScene.addRect(0, 0, self.pixmaps[0].width(), self.pixmaps[0].height())
		t = 0
		for i in range(len(self.wScene.items())-1,-1,-1):
			item = self.wScene.items()[i]

			if isinstance(item, QGraphicsRectItem):
				item.setPos(self.indexToPosition(self.selectedIndex))
				item.setBrush(cursorColor)
				item.setZValue(1)
			else:
				item.setPos(self.indexToPosition(t))
				t += 1
			item.setScale(self.scaleFactor)

