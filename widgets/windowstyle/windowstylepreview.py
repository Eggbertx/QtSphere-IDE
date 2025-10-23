from PySide6.QtCore import Qt, QPoint, QRect
from PySide6.QtGui import QPainter, QMouseEvent
from PySide6.QtWidgets import QWidget
from PySide6.QtGui import QPixmap, QImage

from formats.windowstyle import SphereWindowStyle, WindowStyleBitmap, BackgroundMode


class WindowStylePreview(QWidget):
	windowStyle: SphereWindowStyle
	__showGrid: bool
	activeBitmap: WindowStyleBitmap

	@property
	def ulBitmap(self):
		if self.windowStyle is None:
			return None
		return self.windowStyle.bitmaps[WindowStyleBitmap.UpperLeft]
	
	@property
	def topBitmap(self):
		if self.windowStyle is None:
			return None
		return self.windowStyle.bitmaps[WindowStyleBitmap.Top]
	
	@property
	def urBitmap(self):
		if self.windowStyle is None:
			return None
		return self.windowStyle.bitmaps[WindowStyleBitmap.UpperRight]
	
	@property
	def leftBitmap(self):
		if self.windowStyle is None:
			return None
		return self.windowStyle.bitmaps[WindowStyleBitmap.Left]
	
	@property
	def bgBitmap(self):
		if self.windowStyle is None:
			return None
		return self.windowStyle.bitmaps[WindowStyleBitmap.Background]
	
	@property
	def rightBitmap(self):
		if self.windowStyle is None:
			return None
		return self.windowStyle.bitmaps[WindowStyleBitmap.Right]
	
	@property
	def llBitmap(self):
		if self.windowStyle is None:
			return None
		return self.windowStyle.bitmaps[WindowStyleBitmap.LowerLeft]
	
	@property
	def bottomBitmap(self):
		if self.windowStyle is None:
			return None
		return self.windowStyle.bitmaps[WindowStyleBitmap.Bottom]
	
	@property
	def lrBitmap(self):
		if self.windowStyle is None:
			return None
		return self.windowStyle.bitmaps[WindowStyleBitmap.LowerRight]


	def __init__(self, parent):
		super().__init__(parent)
		self.windowStyle = None
		self.alphaBG = QPixmap(":/res/transparency-bg.png")
		self.__showGrid = True
		self.activeBitmap = WindowStyleBitmap.UpperLeft

	def setGridVisible(self, visible: bool):
		self.__showGrid = visible
		self.update()


	def attachWindowStyle(self, rws: SphereWindowStyle):
		self.windowStyle = rws
		ul = rws.bitmaps[WindowStyleBitmap.UpperLeft]
		top = rws.bitmaps[WindowStyleBitmap.Top]
		ur = rws.bitmaps[WindowStyleBitmap.UpperRight]
		left = rws.bitmaps[WindowStyleBitmap.Left]
		bg = rws.bitmaps[WindowStyleBitmap.Background]
		right = rws.bitmaps[WindowStyleBitmap.Right]
		ll = rws.bitmaps[WindowStyleBitmap.LowerLeft]
		bottom = rws.bitmaps[WindowStyleBitmap.Bottom]
		lr = rws.bitmaps[WindowStyleBitmap.LowerRight]

		minHeightTop = min(ul.height(), top.height(), ur.height())
		minHeightMiddle = min(left.height(), bg.height(), right.height())
		minHeightBottom = min(ll.height(), bottom.height(), lr.height())
		self.setMinimumHeight(minHeightTop + minHeightMiddle + minHeightBottom)

		minWidthLeft = min(ul.width(), left.width(), ll.width())
		minWidthMiddle = min(top.width(), bg.width(), bottom.width())
		minWidthRight = min(ur.width(), right.width(), lr.width())
		self.setMinimumWidth(minWidthLeft + minWidthMiddle + minWidthRight)


	def getColorForBitmap(self, bitmap: WindowStyleBitmap):
		if bitmap == self.activeBitmap:
			return Qt.GlobalColor.green
		else:
			return Qt.GlobalColor.magenta

	def gridRects(self):
		w = self.rect().width()
		h = self.rect().height()
		return (
			QRect(0, 0, self.ulBitmap.width(), self.ulBitmap.height()),
			QRect(self.ulBitmap.width(), 0, w - self.ulBitmap.width() - self.urBitmap.width(), self.topBitmap.height()),
			QRect(w - self.urBitmap.width(), 0, self.urBitmap.width(), self.urBitmap.height()),
			QRect(0, self.ulBitmap.height(), self.leftBitmap.width(), h - self.ulBitmap.height() - self.llBitmap.height()),
			QRect(self.leftBitmap.width(), self.topBitmap.height(), w - self.leftBitmap.width() - self.rightBitmap.width(), h - self.topBitmap.height() - self.bottomBitmap.height()),
			QRect(w - self.rightBitmap.width(), self.urBitmap.height(), self.rightBitmap.width(), h - self.urBitmap.height() - self.lrBitmap.height()),
			QRect(0, h - self.llBitmap.height(), self.llBitmap.width(), self.llBitmap.height()),
			QRect(self.llBitmap.width(), h - self.bottomBitmap.height(), w - self.llBitmap.width() - self.lrBitmap.width(), self.bottomBitmap.height()),
			QRect(w - self.lrBitmap.width(), h - self.lrBitmap.height(), self.lrBitmap.width(), self.lrBitmap.height())
		)

	def paintEvent(self, event):
		painter = QPainter(self)

		w = self.rect().width()
		h = self.rect().height()

		for x in range(0, w, self.topBitmap.width()):
			painter.drawImage(x, 0, self.topBitmap)
		for x in range(0, w, self.bottomBitmap.width()):
			painter.drawImage(x, h - self.bottomBitmap.height(), self.bottomBitmap)
		for y in range(0, h, self.leftBitmap.height()):
			painter.drawImage(0, y, self.leftBitmap)
		for y in range(0, h, self.rightBitmap.height()):
			painter.drawImage(w - self.rightBitmap.width(), y, self.rightBitmap)

		painter.eraseRect(0, 0, self.ulBitmap.width(), self.ulBitmap.height())
		painter.eraseRect(w-self.urBitmap.width(), 0, self.urBitmap.width(), self.urBitmap.height())
		painter.eraseRect(0, h - self.llBitmap.height(), self.llBitmap.width(), self.llBitmap.height())
		painter.eraseRect(w - self.lrBitmap.width(), h - self.lrBitmap.height(), self.lrBitmap.width(), self.lrBitmap.height())

		# corners
		painter.drawImage(0, 0, self.ulBitmap)
		painter.drawImage(w - self.urBitmap.width(), 0, self.urBitmap)
		painter.drawImage(0, h - self.llBitmap.height(), self.llBitmap)
		painter.drawImage(w - self.lrBitmap.width(), h - self.lrBitmap.height(), self.lrBitmap)

		# background
		bgX = self.leftBitmap.width()
		bgY = self.topBitmap.height()
		bgW = w - self.leftBitmap.width() - self.rightBitmap.width()
		bgH = h - self.topBitmap.height() - self.bottomBitmap.height()
		if self.windowStyle.backgroundMode in (BackgroundMode.Stretched, BackgroundMode.StretchedWithGradient):
			scaledBG = self.bgBitmap.scaled(bgW, bgH, Qt.IgnoreAspectRatio, Qt.TransformationMode.FastTransformation)
			painter.drawImage(QPoint(bgX, bgY), scaledBG)
		else:
			for x in range(bgX, bgX + bgW, self.bgBitmap.width()):
				for y in range(bgY, bgY + bgH, self.bgBitmap.height()):
					painter.drawImage(QPoint(x, y), self.bgBitmap)

		# grid
		if not self.__showGrid:
			painter.end()
			return

		gridRects = self.gridRects()

		for r in range(len(gridRects)):
			if r != self.activeBitmap:
				painter.setPen(self.getColorForBitmap(r))
				painter.drawRect(gridRects[r])

		# make sure active grid is on top
		painter.setPen(self.getColorForBitmap(self.activeBitmap))
		painter.drawRect(gridRects[self.activeBitmap])

		painter.end()

	def mousePressEvent(self, event: QMouseEvent):
		if event.button() != Qt.MouseButton.LeftButton:
			return
		
		currentActive = self.activeBitmap
		x = event.position().x()
		y = event.position().y()
		gridRects = self.gridRects()
		for r in range(len(gridRects)):
			if gridRects[r].contains(QPoint(x, y)):
				self.activeBitmap = r
				if currentActive != self.activeBitmap:
					self.update()
				return
