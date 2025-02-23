from PySide6.QtCore import QSize
from PySide6.QtWidgets import QDialog

from ui.ui_resizedialog import Ui_ResizeDialog

class ResizeDialog(QDialog):
	ui: Ui_ResizeDialog

	@property
	def width(self) -> int:
		return self.ui.widthNum.value()


	@width.setter
	def width(self, value: int):
		self.ui.widthNum.setValue(value)


	@property
	def height(self) -> int:
		return self.ui.heightNum.value()


	@height.setter
	def height(self, value: int):
		self.ui.heightNum.setValue(value)


	@property
	def prefix(self) -> str:
		return self.ui.widthNum.prefix()


	@prefix.setter
	def prefix(self, value: str):
		self.ui.widthNum.setPrefix(value)
		self.ui.heightNum.setPrefix(value)


	@property
	def suffix(self) -> str:
		return self.ui.widthNum.suffix()


	@suffix.setter
	def suffix(self, value: str):
		self.ui.widthNum.setSuffix(value)
		self.ui.heightNum.setSuffix(value)


	@property
	def sizeValue(self) -> QSize:
		return QSize(self.width, self.height)


	@sizeValue.setter
	def sizeValue(self, value: QSize):
		self.width = value.width()
		self.height = value.height()


	def __init__(self, title:str, initialSize:QSize, parent = None):
		super().__init__(parent)
		self.ui = Ui_ResizeDialog()
		self.ui.setupUi(self)
		self.ui.buttonBox.accepted.connect(self.accept)
		self.ui.buttonBox.rejected.connect(self.reject)
		self.setWindowTitle(title)
		self.sizeValue = initialSize
