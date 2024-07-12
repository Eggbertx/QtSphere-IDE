from enum import Enum

from PySide6.QtCore import Qt, Signal
from PySide6.QtGui import QUndoStack, QAction
from PySide6.QtWidgets import QWidget

class SphereFile(Enum):
	Text = 0
	Font = 1
	Map = 2
	Package = 3
	Spriteset = 4
	TilesetFile = 5
	WindowStyle = 6

class SphereEditor(QWidget):
	undoStack: QUndoStack
	undoAction: QAction
	redoAction: QAction
	editorType: SphereFile = SphereFile.Text
	filePath:str
	modificationChanged:Signal = Signal(bool)
	__modified:bool

	@staticmethod
	def openAndAttach(parent: QWidget, filePath:str):
		pass

	def __init__(self, parent: QWidget | None = None):
		super().__init__(parent)
		self.filePath = ""
		self.undoStack = QUndoStack(self)
		self.undoAction = QAction("&Undo")
		self.undoAction.setShortcut(Qt.Key.Key_Undo)
		self.redoAction = QAction("&Redo")
		self.redoAction.setShortcut(Qt.Key.Key_Redo)
		self.__modified = False

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