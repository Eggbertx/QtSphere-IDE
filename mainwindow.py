import sys
from PySide6.QtCore import QCoreApplication, QSettings
from PySide6.QtGui import QIcon
from PySide6.QtWidgets import QApplication, QMainWindow, QMessageBox

from ui.ui_mainwindow import Ui_MainWindow
from widgets.startpage import StartPage

_VERSION = "0.9"
_APPLICATION_NAME = "QtSphere IDE"
_ORG_NAME = "Spherical"
_ABOUT_STRING = f"""QtSphere IDE v{_VERSION}<br />
Copyright 2024 by <a href=\"https://github.com/eggbertx\">Eggbertx</a><br /><br />
See <a href=\"https://github.com/Eggbertx/QtSphere-IDE/blob/master/LICENSE.txt\">LICENSE.txt</a> for more information.
"""

class MainWindow(QMainWindow):
	settings: QSettings
	def __init__(self, settings:QSettings=None, parent=None):
		super().__init__(parent)
		self.ui = Ui_MainWindow()
		self.ui.setupUi(self)
		self.settings = settings
		self.ui.splitter.setStretchFactor(1, 4)
		self.setWindowIcon(QIcon(":/icons/res/icon.png"))
		self.startPage = StartPage(self.ui.openFileTabs)
		self.ui.openFileTabs.addTab(self.startPage, "Start Page")
		self.connectActions()
	
	def connectActions(self):
		self.ui.actionExit.triggered.connect(sys.exit)
		self.ui.actionAbout_Qt.triggered.connect(lambda: QMessageBox.aboutQt(self, "About Qt"))
		self.ui.actionAbout.triggered.connect(lambda: QMessageBox.about(self, "About QtSphere IDE", _ABOUT_STRING))


if __name__ == "__main__":
	QCoreApplication.setApplicationName(_APPLICATION_NAME)
	QCoreApplication.setOrganizationName(_ORG_NAME)
	QCoreApplication.setApplicationVersion(_VERSION)
	app = QApplication(sys.argv)

	settings = QSettings()
	window = MainWindow(settings)
	if settings.value("maximized", True):
		window.showMaximized()
	else:
		window.show()
	sys.exit(app.exec())
