from PySide6.QtCore import Qt, QPoint, QRect, Signal
from PySide6.QtGui import QPainter, QMouseEvent
from PySide6.QtWidgets import QWidget
from PySide6.QtGui import QPixmap, QImage

from formats.windowstyle import SphereWindowStyle, WindowStyleBitmap, BackgroundMode


class WindowStylePreview(QWidget):
	windowStyle: SphereWindowStyle
	__showGrid: bool
	activeBitmap: WindowStyleBitmap
	__scale: int
	activeBitmapChanged = Signal(int)

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
		self.__scale = 2

	def setGridVisible(self, visible: bool):
		self.__showGrid = visible
		self.update()

	def setScale(self, scale: int):
		self.__scale = scale
		self.setMinimumSize()
		self.update()

	def setMinimumSize(self):
		s = self.__scale
		minWidthLeft = min(self.ulBitmap.width()*s, self.leftBitmap.width()*s, self.llBitmap.width()*s)
		minWidthMiddle = min(self.topBitmap.width()*s, self.bgBitmap.width()*s, self.bottomBitmap.width()*s)
		minWidthRight = min(self.urBitmap.width()*s, self.rightBitmap.width()*s, self.lrBitmap.width()*s)
		self.setMinimumWidth(minWidthLeft + minWidthMiddle + minWidthRight)

		minHeightTop = min(self.ulBitmap.height()*s, self.topBitmap.height()*s, self.urBitmap.height()*s)
		minHeightMiddle = min(self.leftBitmap.height()*s, self.bgBitmap.height()*s, self.rightBitmap.height()*s)
		minHeightBottom = min(self.llBitmap.height()*s, self.bottomBitmap.height()*s, self.lrBitmap.height()*s)
		self.setMinimumHeight(minHeightTop + minHeightMiddle + minHeightBottom)

	def attachWindowStyle(self, rws: SphereWindowStyle):
		self.windowStyle = rws
		self.setMinimumSize()
		self.update()


	def getColorForBitmap(self, bitmap: WindowStyleBitmap):
		if bitmap == self.activeBitmap:
			return Qt.GlobalColor.green
		else:
			return Qt.GlobalColor.magenta

	def gridRects(self):
		w = self.rect().width()
		h = self.rect().height()
		s = self.__scale
		ulsw = self.ulBitmap.width()*s
		ulsh = self.ulBitmap.height()*s
		tsw = self.topBitmap.width()*s
		tsh = self.topBitmap.height()*s
		ursw = self.urBitmap.width()*s
		ursh = self.urBitmap.height()*s
		lsw = self.leftBitmap.width()*s
		lsh = self.leftBitmap.height()*s
		bgsw = self.bgBitmap.width()*s
		bgsh = self.bgBitmap.height()*s
		rsw = self.rightBitmap.width()*s
		rsh = self.rightBitmap.height()*s
		llsw = self.llBitmap.width()*s
		llsh = self.llBitmap.height()*s
		bsw = self.bottomBitmap.width()*s
		bsh = self.bottomBitmap.height()*s
		lrsw = self.lrBitmap.width()*s
		lrsh = self.lrBitmap.height()*s
		return (
			QRect(0, 0, ulsw, ulsh),
			QRect(ulsw, 0, w - ulsw - ursw, tsh),
			QRect(w - ursw, 0, ursw, ursh),
			QRect(0, ulsh, lsw, h - ulsh - llsh),
			QRect(lsw, tsh, w - lsw - rsw, h - tsh - bsh),
			QRect(w - rsw, ursh, rsw, h - ursh - lrsh),
			QRect(0, h - llsh, llsw, llsh),
			QRect(llsw, h - bsh, w - llsw - lrsw, bsh),
			QRect(w - lrsw, h - lrsh, lrsw, lrsh)
		)

	def paintEvent(self, event):
		painter = QPainter(self)

		w = self.rect().width()
		h = self.rect().height()
		s = self.__scale

		# background
		bgX = self.leftBitmap.width()*s
		bgY = self.topBitmap.height()*s
		bgW = w - self.leftBitmap.width()*s - self.rightBitmap.width()*s
		bgH = h - self.topBitmap.height()*s - self.bottomBitmap.height()*s
		if self.windowStyle.backgroundMode in (BackgroundMode.Stretched, BackgroundMode.StretchedWithGradient):
			scaledBG = self.bgBitmap.scaled(bgW, bgH, Qt.IgnoreAspectRatio, Qt.TransformationMode.FastTransformation)
			painter.drawImage(QPoint(bgX, bgY), scaledBG)
		else:
			# tiled background
			for x in range(bgX, bgX + bgW, self.bgBitmap.width()*s):
				for y in range(bgY, bgY + bgH, self.bgBitmap.height()*s):
					painter.drawImage(QPoint(x, y), self.bgBitmap.scaled(self.bgBitmap.width()*s, self.bgBitmap.height()*s))

		sw = self.topBitmap.width()*s
		sh = self.topBitmap.height()*s
		for x in range(0, w, sw):
			painter.drawImage(x, 0, self.topBitmap.scaled(sw, sh))

		sw = self.bottomBitmap.width()*s
		sh = self.bottomBitmap.height()*s
		for x in range(0, w, sw):
			painter.drawImage(x, h - sh, self.bottomBitmap.scaled(sw, sh))
		sw = self.leftBitmap.width()*s
		sh = self.leftBitmap.height()*s
		for y in range(0, h, sw):
			painter.drawImage(0, y, self.leftBitmap.scaled(sw, sh))
		sw = self.rightBitmap.width()*s
		sh = self.rightBitmap.height()*s
		for y in range(0, h, sh):
			painter.drawImage(w - sw, y, self.rightBitmap.scaled(sw, sh))

		painter.eraseRect(0, 0, self.ulBitmap.width()*s, self.ulBitmap.height()*s)
		painter.eraseRect(w-self.urBitmap.width()*s, 0, self.urBitmap.width()*s, self.urBitmap.height()*s)
		painter.eraseRect(0, h - self.llBitmap.height()*s, self.llBitmap.width()*s, self.llBitmap.height()*s)
		painter.eraseRect(w - self.lrBitmap.width(), h - self.lrBitmap.height()*s, self.lrBitmap.width()*s, self.lrBitmap.height()*s)

		# corners
		painter.drawImage(0, 0, self.ulBitmap.scaled(self.ulBitmap.width()*s, self.ulBitmap.height()*s))
		painter.drawImage(w - self.urBitmap.width()*s, 0, self.urBitmap.scaled(self.urBitmap.width()*s, self.urBitmap.height()*s))
		painter.drawImage(0, h - self.llBitmap.height()*s, self.llBitmap.scaled(self.llBitmap.width()*s, self.llBitmap.height()*s))
		painter.drawImage(w - self.lrBitmap.width()*s, h - self.lrBitmap.height()*s, self.lrBitmap.scaled(self.lrBitmap.width()*s, self.lrBitmap.height()*s))

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

	def setActiveBitmap(self, b: int):
		currentActive = self.activeBitmap
		self.activeBitmap = b
		if currentActive != self.activeBitmap:
			self.update()
			self.activeBitmapChanged.emit(self.activeBitmap)

	def mousePressEvent(self, event: QMouseEvent):
		if event.button() != Qt.MouseButton.LeftButton:
			return
		
		currentActive = self.activeBitmap
		x = event.position().x()
		y = event.position().y()
		gridRects = self.gridRects()
		for r in range(len(gridRects)):
			if gridRects[r].contains(QPoint(x, y)):
				self.setActiveBitmap(r)
				return
