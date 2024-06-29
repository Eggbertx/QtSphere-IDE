from enum import Enum
import os

from PySide6.QtCore import Qt, Slot, Signal, QSettings
from PySide6.QtGui import QColor
from PySide6.QtWidgets import QDialog, QWidget, QDialogButtonBox, QListWidgetItem, QFileDialog, QAbstractButton

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
		self._removeWineIfWindows()
		self._loadSettings()

	def _loadSettings(self):
		settings = QSettings()
		self.ui.mapCursorCol_btn.setColor(QColor(settings.value("mapCursorColor", QColor.fromString("#0080ff"))))
		self.ui.gridColor_btn.setColor(QColor(settings.value("gridColor", QColor.fromString("#000000"))))
		self.ui.wineDir_txt.setText(settings.value("wineDir", "/usr/bin"))
		self.ui.neosphereDir_txt.setText(settings.value("neosphereDir", ""))
		self.ui.legacySphereDir_txt.setText(settings.value("legacySphereDir", ""))
		self.ui.unrecognizedFileEditor_combo.setCurrentIndex(0 if settings.value("unrecognizedFileEditor", "external") == "external" else 1)

		numSearchPaths = settings.beginReadArray("projectDirs")
		for d in range(numSearchPaths):
			settings.setArrayIndex(d)
			directory = settings.value("directory")
			if directory != "":
				self._addProjectDirItem(directory)
		settings.endArray()


	def _saveSettings(self):
		settings = QSettings()
		settings.setValue("mapCursorColor", self.ui.mapCursorCol_btn.color)
		settings.setValue("gridColor", self.ui.gridColor_btn.color)
		if os.name != "nt":
			settings.setValue("wineDir", self.ui.wineDir_txt.text())
		settings.setValue("neosphereDir", self.ui.neosphereDir_txt.text())
		settings.setValue("legacySphereDir", self.ui.legacySphereDir_txt.text())
		settings.setValue("unrecognizedFileEditor", "text" if self.ui.unrecognizedFileEditor_combo.currentIndex() == 1 else "external")

		settings.remove("projectDirs")
		settings.beginWriteArray("projectDirs")
		numDirs = self.ui.projectDirsList.count()
		for d in range(numDirs):
			settings.setArrayIndex(d)
			directory = self.ui.projectDirsList.item(d).text()
			if directory != "" and not self.ui.projectDirsList.item(d).isHidden():
				settings.setValue("directory", directory)
		settings.endArray()
		self.settingsSaved.emit()

	def _addProjectDirItem(self, text:str):
		item = QListWidgetItem(text, self.ui.projectDirsList)
		item.setFlags(item.flags()|Qt.ItemFlag.ItemIsEditable)
		self.ui.projectDirsList.addItem(item)

	def _removeWineIfWindows(self):
		if os.name == "nt":
			# running in Windows, no need for WINE stuff
			settings = QSettings()
			settings.remove("wineDir")
			self.layout().removeWidget(self.ui.wineDir_lbl)
			self.ui.wineDir_layout.removeWidget(self.ui.wineDir_btn)
			self.ui.wineDir_layout.removeWidget(self.ui.wineDir_txt)
			self.ui.wineDir_lbl.deleteLater()
			self.ui.wineDir_btn.deleteLater()
			self.ui.wineDir_txt.deleteLater()
			self.ui.wineDir_layout.deleteLater()


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
