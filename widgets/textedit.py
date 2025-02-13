from PySide6.QtCore import Qt, Signal, Slot
from PySide6.QtWidgets import QTextEdit, QWidget

from widgets.sphereeditor import SphereEditorType

class TextEdit(QTextEdit):
	filePath:str
	modificationChanged:Signal = Signal(bool)

	@staticmethod
	def openAndAttach(parent: QWidget, filePath:str):
		te = TextEdit(parent)
		with open(filePath, "rb") as file:
			te.setText(file.read().decode("utf-8", "replace"))
			te.document().setModified(False)
			te.filePath = filePath
		return te

	@property
	def editorType(self):
		return SphereEditorType.Text

	def __init__(self, parent: QWidget = None):
		super().__init__(parent)
		self.filePath = ""
		self.setTabStopDistance(self.tabStopDistance()/2)
		self.document().setModified(False)
		self.textChanged.connect(self.__onTextChanged)
		self.document().modificationChanged.connect(self.__onDocumentModificationChanged)

	def save(self, filePath = ""):
		if filePath == "":
			filePath = self.filePath
		with open(filePath, "w") as file:
			file.write(self.toPlainText())
			self.document().setModified(False)

	def isModified(self):
		return self.document().isModified()

	def setModified(self, modified:bool):
		self.document().setModified(modified)

	@Slot(bool)
	def __onDocumentModificationChanged(self, modified:bool):
		self.modificationChanged.emit(modified)

	@Slot()
	def __onTextChanged(self):
		self.modificationChanged.emit(True)