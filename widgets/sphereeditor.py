from enum import Enum, auto

from PySide6.QtCore import Qt, Signal, QPoint
from PySide6.QtGui import QUndoStack, QAction
from PySide6.QtWidgets import QWidget

class SphereEditorType(Enum):
	Other = -1
	Text = auto()
	Font = auto()
	Map = auto()
	Package = auto()
	Spriteset = auto()
	TilesetFile = auto()
	WindowStyle = auto()
	Image = auto()

class SphereEditor(QWidget):
	undoStack: QUndoStack
	undoAction: QAction
	redoAction: QAction
	filePath:str
	modificationChanged:Signal = Signal(bool)
	__modified:bool

	@staticmethod
	def openAndAttach(parent: QWidget, filePath:str):
		pass


	@staticmethod
	def pixelsInLine(p0:QPoint, p1:QPoint) -> list[QPoint]:
		"""
		Uses Bresenhan's algorithm to calculate the pixels in a line between the two given points, and can be used
		for images or maps (or anything with raster editing)
		"""
		points:list[QPoint] = []
		(x0, y0) = (p0.x(), p0.y())
		(x1, y1) = (p1.x(), p1.y())

		dx = abs(x1 - x0)
		dy = abs(y1 - y0)
		sx = 1 if x0 < x1 else -1
		sy = 1 if y0 < y1 else -1

		err = dx - dy

		while True:
			points.append(QPoint(x0, y0))
			if x0 == x1 and y0 == y1:
				break

			e2 = 2 * err
			if e2 > -dy:
				err -= dy
				x0 += sx
			if e2 < dx:
				err += dx
				y0 += sy
		return points

	__editorType: SphereEditorType
	@property
	def editorType(self):
		return self.__editorType

	def __init__(self, parent: QWidget | None = None, editorType:SphereEditorType = SphereEditorType.Text):
		super().__init__(parent)
		self.filePath = ""
		self.undoStack = QUndoStack(self)
		self.undoAction = QAction("&Undo")
		self.undoAction.setShortcut(Qt.Key.Key_Undo)
		self.redoAction = QAction("&Redo")
		self.redoAction.setShortcut(Qt.Key.Key_Redo)
		self.__modified = False
		self.__editorType = editorType

	def undo(self):
		self.undoStack.undo()
		self.setModified(True)
	
	def redo(self):
		self.undoStack.redo()

	def isModified(self):
		return self.__modified

	def setModified(self, modified:bool):
		oldModified = self.__modified
		self.__modified = modified
		if self.__modified != oldModified:
			self.modificationChanged.emit(modified)
	
	def save(self, newFilePath:str = None):
		pass