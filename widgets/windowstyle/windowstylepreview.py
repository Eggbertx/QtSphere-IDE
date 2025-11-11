from PySide6.QtCore import QPoint, QRect, Qt, Signal
from PySide6.QtGui import QImage, QMouseEvent, QPainter, QPixmap
from PySide6.QtWidgets import QWidget

from formats.windowstyle import BackgroundMode, SphereWindowStyle, WindowStyleBitmap


class WindowStylePreview(QWidget):
	windowStyle: SphereWindowStyle
	__showGrid: bool
	activeBitmap: WindowStyleBitmap
	__scale: int
	activeBitmapChanged = Signal(int)

	@property
	def ulBitmap(self):
		return self.windowStyle.bitmaps[WindowStyleBitmap.UpperLeft]

	@property
	def topBitmap(self):
		return self.windowStyle.bitmaps[WindowStyleBitmap.Top]

	@property
	def urBitmap(self):
		return self.windowStyle.bitmaps[WindowStyleBitmap.UpperRight]

	@property
	def leftBitmap(self):
		return self.windowStyle.bitmaps[WindowStyleBitmap.Left]

	@property
	def bgBitmap(self):
		return self.windowStyle.bitmaps[WindowStyleBitmap.Background]

	@property
	def rightBitmap(self):
		return self.windowStyle.bitmaps[WindowStyleBitmap.Right]

	@property
	def llBitmap(self):
		return self.windowStyle.bitmaps[WindowStyleBitmap.LowerLeft]

	@property
	def bottomBitmap(self):
		return self.windowStyle.bitmaps[WindowStyleBitmap.Bottom]

	@property
	def lrBitmap(self):
		return self.windowStyle.bitmaps[WindowStyleBitmap.LowerRight]

	def __init__(self, parent):
		super().__init__(parent)
		self.windowStyle = SphereWindowStyle()
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
		if self.windowStyle is None:
			return
		s = self.__scale
		minWidthLeft = max(self.ulBitmap.width() * s, self.leftBitmap.width() * s, self.llBitmap.width() * s)
		minWidthMiddle = max(self.topBitmap.width() * s, self.bgBitmap.width() * s, self.bottomBitmap.width() * s)
		minWidthRight = max(self.urBitmap.width() * s, self.rightBitmap.width() * s, self.lrBitmap.width() * s)
		self.setMinimumWidth(minWidthLeft + minWidthMiddle + minWidthRight)

		minHeightTop = max(self.ulBitmap.height() * s, self.topBitmap.height() * s, self.urBitmap.height() * s)
		minHeightMiddle = max(self.leftBitmap.height() * s, self.bgBitmap.height() * s, self.rightBitmap.height() * s)
		minHeightBottom = max(self.llBitmap.height() * s, self.bottomBitmap.height() * s, self.lrBitmap.height() * s)
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
		if self.windowStyle is None:
			return (
				QRect(0, 0, 0, 0),  # upper left
				QRect(0, 0, 0, 0),  # top
				QRect(0, 0, 0, 0),  # upper right
				QRect(0, 0, 0, 0),  # right
				QRect(0, 0, 0, 0),  # lower right
				QRect(0, 0, 0, 0),  # bottom
				QRect(0, 0, 0, 0),  # lower left
				QRect(0, 0, 0, 0),  # left
				QRect(0, 0, 0, 0),  # background
			)
		w = self.rect().width()
		h = self.rect().height()
		s = self.__scale
		ulsw = self.ulBitmap.width() * s
		ulsh = self.ulBitmap.height() * s
		# tsw = self.topBitmap.width()*s
		tsh = self.topBitmap.height() * s
		ursw = self.urBitmap.width() * s
		ursh = self.urBitmap.height() * s
		lsw = self.leftBitmap.width() * s
		# lsh = self.leftBitmap.height()*s
		rsw = self.rightBitmap.width() * s
		# rsh = self.rightBitmap.height()*s
		llsw = self.llBitmap.width() * s
		llsh = self.llBitmap.height() * s
		# bsw = self.bottomBitmap.width()*s
		bsh = self.bottomBitmap.height() * s
		lrsw = self.lrBitmap.width() * s
		lrsh = self.lrBitmap.height() * s

		leftWidth = max(ulsw, lsw, llsw)
		topHeight = max(ulsh, tsh, ursh)
		rightWidth = max(ursw, rsw, lrsw)
		bottomHeight = max(llsh, bsh, lrsh)

		return (
			QRect(0, 0, leftWidth, topHeight),  # upper left
			QRect(leftWidth, 0, w - leftWidth - rightWidth, topHeight),  # top
			QRect(w - rightWidth, 0, rightWidth, topHeight),  # upper right
			QRect(w - rightWidth, topHeight, rightWidth, h - topHeight - bottomHeight),  # right
			QRect(w - rightWidth, h - bottomHeight, rightWidth, bottomHeight),  # lower right
			QRect(leftWidth, h - bottomHeight, w - leftWidth - rightWidth, bottomHeight),  # bottom
			QRect(0, h - bottomHeight, leftWidth, bottomHeight),  # lower left
			QRect(0, topHeight, leftWidth, h - topHeight - bottomHeight),  # left
			QRect(leftWidth, topHeight, w - leftWidth - rightWidth, h - topHeight - bottomHeight),  # background
		)

	def paintEvent(self, event):
		if self.windowStyle is None:
			return
		painter = QPainter(self)
		s = self.__scale
		painter.drawTiledPixmap(0, 0, self.width(), self.height(), self.alphaBG)

		gridRects = self.gridRects()

		# background
		bgX = gridRects[WindowStyleBitmap.Background].x()
		bgY = gridRects[WindowStyleBitmap.Background].y()
		bgW = gridRects[WindowStyleBitmap.Background].width()
		bgH = gridRects[WindowStyleBitmap.Background].height()
		if self.windowStyle.backgroundMode in (BackgroundMode.Stretched, BackgroundMode.StretchedWithGradient):
			scaledBG = self.bgBitmap.scaled(bgW, bgH, Qt.AspectRatioMode.IgnoreAspectRatio, Qt.TransformationMode.FastTransformation)
			painter.drawImage(bgX, bgY, scaledBG)
		else:
			# tiled background
			scaledBG = self.bgBitmap.scaled(self.bgBitmap.width()*s, self.bgBitmap.height()*s)
			painter.drawTiledPixmap(gridRects[WindowStyleBitmap.Background], QPixmap(scaledBG))

		scaled = self.topBitmap.scaled(self.topBitmap.width() * s, self.topBitmap.height() * s)
		painter.drawTiledPixmap(
			gridRects[WindowStyleBitmap.Top].left(),
			gridRects[WindowStyleBitmap.Background].top() - scaled.height(),
			gridRects[WindowStyleBitmap.Top].width(),
			scaled.height(),
			QPixmap(scaled),
			0, scaled.height()
		)

		scaled = self.bottomBitmap.scaled(self.bottomBitmap.width() * s, self.bottomBitmap.height() * s)
		painter.drawTiledPixmap(
			gridRects[WindowStyleBitmap.Bottom].left(),
			gridRects[WindowStyleBitmap.Bottom].top(),
			gridRects[WindowStyleBitmap.Bottom].width(),
			scaled.height(),
			QPixmap(scaled),
			0, scaled.height()
		)

		scaled = self.leftBitmap.scaled(self.leftBitmap.width() * s, self.leftBitmap.height() * s)
		painter.drawTiledPixmap(
			gridRects[WindowStyleBitmap.Left].right() - scaled.width() + 1,
			gridRects[WindowStyleBitmap.Left].y(),
			scaled.width(),
			gridRects[WindowStyleBitmap.Left].height(),
			QPixmap(scaled),
			scaled.width(), 0
		)

		scaled = self.rightBitmap.scaled(self.rightBitmap.width() * s, self.rightBitmap.height() * s)
		painter.drawTiledPixmap(
			gridRects[WindowStyleBitmap.Right].x(),
			gridRects[WindowStyleBitmap.Right].y(),
			scaled.width(),
			gridRects[WindowStyleBitmap.Right].height(),
			QPixmap(scaled),
			scaled.width(), 0
		)


		# corners
		scaled = self.ulBitmap.scaled(self.ulBitmap.width() * s, self.ulBitmap.height() * s)
		painter.drawImage(
			gridRects[WindowStyleBitmap.UpperLeft].width() - self.ulBitmap.width() * s,
			gridRects[WindowStyleBitmap.UpperLeft].height() - self.ulBitmap.height() * s,
			scaled
		)

		scaled = self.urBitmap.scaled(self.urBitmap.width() * s, self.urBitmap.height() * s)
		painter.drawImage(
			gridRects[WindowStyleBitmap.UpperRight].x(),
			gridRects[WindowStyleBitmap.Right].top() - scaled.height(),
			scaled
		)

		scaled = self.llBitmap.scaled(self.llBitmap.width() * s, self.llBitmap.height() * s)
		painter.drawImage(
			gridRects[WindowStyleBitmap.LowerLeft].right() - scaled.width() + 1,
			gridRects[WindowStyleBitmap.LowerLeft].y(),
			scaled
		)

		scaled = self.lrBitmap.scaled(self.lrBitmap.width() * s, self.lrBitmap.height() * s)
		painter.drawImage(
			gridRects[WindowStyleBitmap.LowerRight].x(),
			gridRects[WindowStyleBitmap.LowerRight].y(),
			scaled
		)

		if not self.__showGrid:
			painter.end()
			return

		for r in range(len(gridRects)):
			if r != self.activeBitmap:
				painter.setPen(self.getColorForBitmap(r))
				painter.drawRect(gridRects[r])

		# make sure active grid is on top
		painter.setPen(self.getColorForBitmap(self.activeBitmap))
		painter.drawRect(gridRects[self.activeBitmap])

		painter.end()

	def setActiveBitmap(self, b: int):
		if self.activeBitmap != b:
			self.activeBitmap = b
			self.update()
			self.activeBitmapChanged.emit(self.activeBitmap)

	def mousePressEvent(self, event: QMouseEvent):
		if event.button() != Qt.MouseButton.LeftButton:
			return

		x = event.position().x()
		y = event.position().y()
		gridRects = self.gridRects()
		for r in range(len(gridRects)):
			if gridRects[r].contains(QPoint(x, y)):
				self.setActiveBitmap(r)
				return
