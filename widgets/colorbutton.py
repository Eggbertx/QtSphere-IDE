from typing import override
from PySide6.QtGui import QColor, QColorConstants, QMouseEvent, QPixmap, QPainter
from PySide6.QtCore import QSize, Qt, Signal, QRect
from PySide6.QtWidgets import QToolButton, QWidget, QStyle, QColorDialog


class ColorButton(QToolButton):
	color: QColor
	colorChanged: Signal = Signal(QColor)
	colorIcon: QPixmap
	pixmapSize: QSize
	alphaChannel: bool = False

	def __init__(self, parent: QWidget | None = None, color: QColor = QColorConstants.White) -> None:
		super().__init__(parent)
		self.color = color
		metricSize = self.style().pixelMetric(QStyle.PixelMetric.PM_ButtonIconSize)
		self.pixmapSize = QSize(metricSize * 2, metricSize * 2)
		self.updateIcon()

	def setColor(self, color: QColor):
		if self.color == color or not color.isValid():
			return
		self.color = color
		self.colorChanged.emit(self.color)
		self.updateIcon()

	@override
	def mouseReleaseEvent(self, event: QMouseEvent) -> None:
		if event.button() != Qt.MouseButton.LeftButton:
			return

		options = (
			QColorDialog.ColorDialogOption.ShowAlphaChannel
			if self.alphaChannel
			else QColorDialog.ColorDialogOption(0)
		)
		newCol = QColorDialog.getColor(self.color, self, options=options)
		self.setDown(False)
		self.setColor(newCol)
		return super().mouseReleaseEvent(event)

	def updateIcon(self):
		self.colorIcon = QPixmap(self.pixmapSize)
		painter = QPainter(self.colorIcon)
		if self.alphaChannel and self.color.alpha() < 255:
			alphaPixmap = QPixmap(":/res/transparency-bg.png")
			for y in range(0, self.pixmapSize.height(), alphaPixmap.height()):
				for x in range(0, self.pixmapSize.width(), alphaPixmap.width()):
					painter.drawPixmap(x, y, alphaPixmap)
		painter.fillRect(QRect(0, 0, self.pixmapSize.width(), self.pixmapSize.height()), self.color)
		painter.end()

		self.setIconSize(self.pixmapSize)
		self.setIcon(self.colorIcon)
