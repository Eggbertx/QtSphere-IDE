from PySide6.QtCore import Qt, Slot, Signal
from PySide6.QtWidgets import QDialog, QWidget, QTreeWidgetItem, QAbstractButton, QDialogButtonBox

from ui.ui_modifiedfilesdialog import Ui_ModifiedFilesDialog

from widgets.maintabwidget import MainTabWidget

class ModifiedFilesDialog(QDialog):
	ui: Ui_ModifiedFilesDialog
	paths: list[str]

	@property
	def buttonBox(self):
		return self.ui.buttonBox

	def __init__(self, parent: QWidget):
		super().__init__(parent)
		self.ui = Ui_ModifiedFilesDialog()
		self.ui.setupUi(self)
		self.paths = []

	def clearPaths(self):
		self.ui.treeWidget.clear()
		self.paths.clear()
	
	def addPath(self, path:str):
		item = QTreeWidgetItem(self.ui.treeWidget)
		item.setText(0, path)
		self.paths.append(path)
		self.ui.treeWidget.insertTopLevelItem(0, item)

	def setPaths(self, paths:list[str]):
		self.clearPaths()
		for path in paths:
			self.addPath(path)
