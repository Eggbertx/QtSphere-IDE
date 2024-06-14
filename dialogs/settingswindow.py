import os

from PySide6.QtCore import Qt, Slot, QSettings
from PySide6.QtGui import QColor
from PySide6.QtWidgets import QDialog, QWidget, QDialogButtonBox

from ui.ui_settingswindow import Ui_SettingsWindow

class SettingsWindow(QDialog):
	ui: Ui_SettingsWindow
	def __init__(self, parent: QWidget|None = None, wtype: Qt.WindowType = Qt.WindowType.Widget):
		super().__init__(parent, wtype)
		self.ui = Ui_SettingsWindow()
		self.ui.setupUi(self)

		settings = QSettings()
		self.ui.mapCursorCol_btn.setColor(QColor(settings.value("mapCursorColor", QColor.fromString("#0080ff"))))
		self.ui.gridColor_btn.setColor(QColor(settings.value("gridColor", QColor.fromString("#000000"))))
		self.ui.winePath_txt.setText(settings.value("winePath", "/usr/bin/wine"))
		self.ui.neosphereDir_txt.setText(settings.value("neosphereDir", ""))
		self.ui.legacySphereDir_txt.setText(settings.value("legacySphereDir", ""))

		self.ui.buttonBox.button(QDialogButtonBox.StandardButton.Ok).clicked.connect(self.onOK)
		self.ui.buttonBox.button(QDialogButtonBox.StandardButton.Apply).clicked.connect(self.onApply)
		self.ui.buttonBox.button(QDialogButtonBox.StandardButton.Cancel).clicked.connect(self.reject)

		self._removeWineIfWindows()


	def _saveSettings(self):
		settings = QSettings()
		settings.setValue("mapCursorColor", self.ui.mapCursorCol_btn.color)
		settings.setValue("gridColor", self.ui.gridColor_btn.color)


	def _removeWineIfWindows(self):
		if os.name == "nt":
			# running in Windows, no need for WINE stuff
			settings = QSettings()
			settings.remove("winePath")
			self.layout().removeWidget(self.ui.winePath_lbl)
			self.ui.winePath_layout.removeWidget(self.ui.winePath_btn)
			self.ui.winePath_layout.removeWidget(self.ui.winePath_txt)
			self.ui.winePath_lbl.deleteLater()
			self.ui.winePath_btn.deleteLater()
			self.ui.winePath_txt.deleteLater()
			self.ui.winePath_layout.deleteLater()

	@Slot()	
	def onOK(self):
		self._saveSettings()
		self.accept()

	@Slot()	
	def onApply(self):
		self._saveSettings()

