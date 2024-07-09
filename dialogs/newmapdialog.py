from os import curdir
from typing import Any

from PySide6.QtCore import Qt, Slot
from PySide6.QtWidgets import QDialog, QWidget, QFileDialog, QMessageBox

from ui.ui_newmapdialog import Ui_NewMapDialog

_TILESET_FILE_FILTER = (
	"Sphere tileset (*.rts)",
	"All files (*.*)"
)

_DEFAULT_TILES_W = 64
_DEFAULT_TILES_H = 64

class NewMapDialog(QDialog):
	ui: Ui_NewMapDialog
	tilesetDialog: QFileDialog
	projectPath: str

	@property
	def tilesW(self):
		return self.ui.width_num.value()

	@property
	def tilesH(self):
		return self.ui.height_num.value()

	@property
	def tilesetPath(self):
		return self.ui.browse_txt.text()

	@tilesetPath.setter
	def tilesetPath(self, path:str):
		self.ui.browse_txt.setText(path)

	def __init__(self, parent: QWidget | None = None, projectPath:str = None):
		super().__init__(parent)
		self.ui = Ui_NewMapDialog()
		self.ui.setupUi(self)
		self.projectPath = projectPath
		self.ui.browse_btn.clicked.connect(self.showTilesetBrowseDialog)

	def accept(self):
		if self.tilesetPath == "":
			if QMessageBox.question(self, "No tileset selected",
				"This will create an empty, internal tileset. Are you sure you want to continue?",
				defaultButton=QMessageBox.StandardButton.Yes) != QMessageBox.StandardButton.Yes:
				self.reject()
				return
		super().accept()

	def show(self):
		self.ui.width_num.setValue(_DEFAULT_TILES_W)
		self.ui.height_num.setValue(_DEFAULT_TILES_H)
		self.ui.browse_txt.setText("")
		return super().show()

	@Slot()
	def showTilesetBrowseDialog(self):
		result = QFileDialog.getOpenFileName(self, "Select a Sphere tileset", self.projectPath or curdir,
			";;".join(_TILESET_FILE_FILTER), _TILESET_FILE_FILTER[0])
		if result[0] is not None and result[0] != "":
			self.ui.browse_txt.setText(result[0])