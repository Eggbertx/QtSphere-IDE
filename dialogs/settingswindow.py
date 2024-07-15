from enum import Enum
import os

from PySide6.QtCore import Qt, Slot, Signal
from PySide6.QtGui import QColor, QGuiApplication
from PySide6.QtWidgets import QDialog, QWidget, QDialogButtonBox, QListWidgetItem, QFileDialog, QMenu, QMessageBox

from settings import Settings, Defaults

from widgets.colorbutton import ColorButton

from ui.ui_settingswindow import Ui_SettingsWindow

class UnrecognizedFileHandler(Enum):
	ExternalEditor = 0,
	InternalTextEditor = 1

class SettingsWindow(QDialog):
	ui: Ui_SettingsWindow
	settingsSaved: Signal = Signal()
	def __init__(self, parent: QWidget|None = None, wtype: Qt.WindowType = Qt.WindowType.Widget):
		super().__init__(parent, wtype)
		self.ui = Ui_SettingsWindow()
		self.ui.setupUi(self)

		self.ui.buttonBox.button(QDialogButtonBox.StandardButton.Ok).clicked.connect(self.onOK)
		self.ui.buttonBox.button(QDialogButtonBox.StandardButton.Apply).clicked.connect(self.onApply)
		self.ui.buttonBox.button(QDialogButtonBox.StandardButton.Cancel).clicked.connect(self.reject)
		self.ui.addDirButton.clicked.connect(self.onAddDirButtonClicked)
		self.ui.removeDirButton.clicked.connect(self.onRemoveDirButtonClicked)
		self.ui.browseDirButton.clicked.connect(self.onBrowseDirButtonClicked)
		self.ui.neosphereDir_btn.clicked.connect(self.onNeoSphereDirButtonClicked)
		self.ui.legacySphereDir_btn.clicked.connect(self.onLegacySphereDirButtonClicked)
		
		self._addColorMenu(self.ui.gridColor_btn)
		self._addColorMenu(self.ui.mapCursorCol_btn)
		self._removeWineIfWindows()
		self.loadSettings()

	def _addColorMenu(self, btn:ColorButton):
		menu = QMenu(btn)
		menu.addAction("Copy color to clipboard").triggered.connect(lambda: QGuiApplication.clipboard().setText(btn.color.name()))
		menu.addAction("Reset color").triggered.connect(lambda: self._resetColorPressed(btn))
		btn.customContextMenuRequested.connect(lambda pt: menu.exec(btn.mapToGlobal(pt)))
	
	def loadSettings(self):
		settings = Settings()
		self.ui.mapCursorCol_btn.setColor(QColor(settings.mapCursorColor))
		self.ui.gridColor_btn.setColor(QColor(settings.gridColor))
		if os.name != "nt":
			self.ui.wineDir_txt.setText(settings.wineDir or "/usr/bin")
		self.ui.neosphereDir_txt.setText(settings.neosphereDir or "")
		self.ui.legacySphereDir_txt.setText(settings.legacySphereDir or "")
		self.ui.unrecognizedFileEditor_combo.setCurrentIndex(0 if settings.unrecognizedFileEditor == "external" else 1)
		self.ui.whichEngine_combo.setCurrentIndex(1 if settings.whichEngine == "legacy" else 0)

		self.ui.projectDirsList.clear()
		searchPaths = settings.projectDirs
		for sPath in searchPaths:
			if sPath != "":
				self._addProjectDirItem(sPath)

	def _saveSettings(self):
		settings = Settings()
		settings.mapCursorColor = self.ui.mapCursorCol_btn.color
		settings.gridColor = self.ui.gridColor_btn.color
		if os.name != "nt":
			settings.wineDir = self.ui.wineDir_txt.text()
		settings.neosphereDir = self.ui.neosphereDir_txt.text()
		settings.legacySphereDir = self.ui.legacySphereDir_txt.text()
		settings.unrecognizedFileEditor = "text" if self.ui.unrecognizedFileEditor_combo.currentIndex() == 1 else "external"
		settings.whichEngine = "legacy" if self.ui.whichEngine_combo.currentIndex() == 1 else "neosphere"

		del settings.projectDirs
		numDirs = self.ui.projectDirsList.count()
		dirs = []
		for d in range(numDirs):
			directory = self.ui.projectDirsList.item(d).text()
			if directory != "" and not self.ui.projectDirsList.item(d).isHidden():
				dirs.append(directory)
		settings.projectDirs = dirs
		self.settingsSaved.emit()

	def _addProjectDirItem(self, text:str):
		item = QListWidgetItem(text, self.ui.projectDirsList)
		item.setFlags(item.flags()|Qt.ItemFlag.ItemIsEditable)
		self.ui.projectDirsList.addItem(item)

	def _removeWineIfWindows(self):
		if os.name == "nt":
			# running in Windows, no need for WINE stuff
			settings = Settings()
			del settings.wineDir
			self.layout().removeWidget(self.ui.wineDir_lbl)
			self.ui.wineDir_layout.removeWidget(self.ui.wineDir_btn)
			self.ui.wineDir_layout.removeWidget(self.ui.wineDir_txt)
			self.ui.wineDir_lbl.deleteLater()
			self.ui.wineDir_btn.deleteLater()
			self.ui.wineDir_txt.deleteLater()
			self.ui.wineDir_layout.deleteLater()

	def _resetColorPressed(self, btn:ColorButton):
		match btn:
			case self.ui.mapCursorCol_btn:
				btn.setColor(Defaults.mapCursorColor.value)
			case self.ui.gridColor_btn:
				btn.setColor(Defaults.gridColor.value)
			case _:
				QMessageBox.critical(self, "Error", f"Unrecognized color button '{btn.objectName()}'")

	@Slot()
	def onOK(self):
		self._saveSettings()
		self.accept()

	@Slot()	
	def onApply(self):
		self._saveSettings()

	@Slot()
	def onAddDirButtonClicked(self):
		self._addProjectDirItem("")
		self.ui.projectDirsList.setCurrentRow(self.ui.projectDirsList.count()-1)

	@Slot()
	def onRemoveDirButtonClicked(self):
		self.ui.projectDirsList.takeItem(self.ui.projectDirsList.currentRow())

	@Slot()
	def onBrowseDirButtonClicked(self):
		row = self.ui.projectDirsList.currentRow()
		if row < 0:
			return
		dir = QFileDialog.getExistingDirectory(self, "Open Directory")
		if dir != "":
			self.ui.projectDirsList.item(row).setText(dir)

	@Slot()
	def onNeoSphereDirButtonClicked(self):
		dir = QFileDialog.getExistingDirectory(self, "Open Directory")
		if dir != "":
			self.ui.neosphereDir_txt.setText(dir)

	@Slot()
	def onLegacySphereDirButtonClicked(self):
		dir = QFileDialog.getExistingDirectory(self, "Open Directory")
		if dir != "":
			self.ui.legacySphereDir_txt.setText(dir)
