from PySide6.QtCore import Qt
from PySide6.QtGui import QMouseEvent
from PySide6.QtWidgets import QTabWidget, QTabBar

class MainTabBar(QTabBar):
	def __init__(self, parent=None):
		super().__init__(parent)
		self.setTabsClosable(True)
	
	def mouseReleaseEvent(self, event: QMouseEvent):
		if event.button() == Qt.MouseButton.MiddleButton:
			index = self.tabAt(event.pos())
			if index > -1:
				self.tabCloseRequested.emit(index)
		event.accept()
		return super().mouseReleaseEvent(event)


class MainTabWidget(QTabWidget):
	def __init__(self, parent=None):
		super().__init__(parent)
		self.setTabBar(MainTabBar(self))
