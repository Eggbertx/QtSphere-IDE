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
		self.ui.buttonBox.button(QDialogButtonBox.StandardButton.Ok).clicked.connect(self.onOK)
		self.ui.buttonBox.button(QDialogButtonBox.StandardButton.Apply).clicked.connect(self.onApply)
		self.ui.buttonBox.button(QDialogButtonBox.StandardButton.Cancel).clicked.connect(self.reject)
	
	def _saveSettings(self):
		settings = QSettings()
		settings.setValue("mapCursorColor", self.ui.mapCursorCol_btn.color)
		settings.setValue("gridColor", self.ui.gridColor_btn.color)

	@Slot()	
	def onOK(self):
		self._saveSettings()
		self.accept()

	@Slot()	
	def onApply(self):
		self._saveSettings()

