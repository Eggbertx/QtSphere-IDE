from PySide6.QtGui import QColor, QColorConstants, QMouseEvent, QPixmap
from PySide6.QtCore import QSize, Qt, Signal
from PySide6.QtWidgets import QToolButton, QWidget, QStyle, QColorDialog

class ColorButton(QToolButton):
	color: QColor
	colorChanged: Signal = Signal(QColor)
	colorIcon: QPixmap
	pixmapSize: QSize

	def __init__(self, parent: QWidget | None = None, color: QColor = QColorConstants.White) -> None:
		super().__init__(parent)
		self.color = color
		metricSize = self.style().pixelMetric(QStyle.PixelMetric.PM_ButtonIconSize)
		self.pixmapSize = QSize(metricSize*2, metricSize)
		self.updateIcon()

	def setColor(self, color: QColor):
		if self.color == color or not color.isValid():
			return
		self.color = color
		self.colorChanged.emit(self.color)
		self.updateIcon()
	
	def mouseReleaseEvent(self, event: QMouseEvent) -> None:
		if event.button() != Qt.MouseButton.LeftButton:
			return
		newCol = QColorDialog.getColor(self.color, self)
		self.setDown(False)
		self.setColor(newCol)
		return super().mouseReleaseEvent(event)
	
	def updateIcon(self):
		self.colorIcon = QPixmap(self.pixmapSize)
		self.colorIcon.fill(QColor(self.color))
		self.setIconSize(self.pixmapSize)
		self.setIcon(self.colorIcon)