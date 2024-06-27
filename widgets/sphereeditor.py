from enum import Enum

from PySide6.QtCore import Qt, QObject
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
	_tabIndex: int

	def __init__(self, parent: QWidget | None = None, wtype: Qt.WindowType = Qt.WindowType.Widget):
		super().__init__(parent, wtype)
		self._tabIndex = -1
		
		self.undoStack = QUndoStack(self)
		self.undoAction = QAction("&Undo")
		self.undoAction.setShortcut(Qt.Key.Key_Undo)
		self.redoAction = QAction("&Redo")
		self.redoAction.setShortcut(Qt.Key.Key_Redo)

	def undo(self):
		self.undoStack.undo()
	
	def redo(self):
		self.undoStack.redo()