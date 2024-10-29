from PySide6.QtCore import Qt
from PySide6.QtGui import QColor
from PySide6.QtWidgets import QDialog, QWidget

from ui.ui_newimagedialog import Ui_NewImageDialog

class NewImageDialog(QDialog):
	ui: Ui_NewImageDialog

	@property
	def imageWidth(self) -> int:
		return self.ui.width_num.value()

	@property
	def imageHeight(self) -> int:
		return self.ui.height_num.value()

	@property
	def fillColor(self) -> QColor:
		return self.ui.fill_btn.color

	@property
	def fillOpacity(self) -> int:
		return self.ui.opacity_num.value()

	def __init__(self, parent: QWidget | None = None,):
		super().__init__(parent)
		self.ui = Ui_NewImageDialog()
		self.ui.setupUi(self)