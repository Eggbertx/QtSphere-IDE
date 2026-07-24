from PySide6.QtWidgets import QDialog
from PySide6.QtCore import QResource

from ui.ui_aboutdialog import Ui_AboutDialog

class AboutDialog(QDialog):
	ui: Ui_AboutDialog
	version: str
	licenseText: str

	def __init__(self, version:str, parent = None):
		super().__init__(parent)
		self.ui = Ui_AboutDialog()
		self.ui.setupUi(self)
		self.version = version
		self.ui.headerLabel.setText(self.ui.headerLabel.text().replace("{version}", self.version))
		res = QResource(":/LICENSE.txt")
		self.licenseText = res.uncompressedData().toStdString()
		self.ui.licenseLabel.setText(self.licenseText)
