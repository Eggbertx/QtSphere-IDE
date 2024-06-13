import sys
from PySide6.QtCore import QCoreApplication, QSettings
from PySide6.QtGui import QIcon
from PySide6.QtWidgets import QApplication, QMainWindow, QMessageBox

# Important:
# For each .ui file (for example, form.ui), you must run following command to generate the ui_form.py file
#    pyside6-uic form.ui -o ui_form.py, or
#    pyside2-uic form.ui -o ui_form.py
from ui.ui_mainwindow import Ui_MainWindow

_VERSION = "0.9"
_APPLICATION_NAME = "QtSphere IDE"
_ORG_NAME = "Spherical"

class MainWindow(QMainWindow):
	settings: QSettings
	def __init__(self, settings:QSettings=None, parent=None):
		super().__init__(parent)
		self.ui = Ui_MainWindow()
		self.ui.setupUi(self)
		self.settings = settings
		self.ui.splitter.setStretchFactor(1, 4)
		self.setWindowIcon(QIcon(":/icons/res/icon.png"))
		self.connectActions()
	
	def connectActions(self):
		self.ui.actionExit.triggered.connect(sys.exit)
		self.ui.actionAbout_Qt.triggered.connect(lambda: QMessageBox.aboutQt(self, "About Qt"))
		self.ui.actionAbout.triggered.connect(lambda: QMessageBox.about(self, "About QtSphere IDE",
		"QtSphere IDE v" + _VERSION + "<br />" +
		"Copyright 2024 by <a href=\"https://github.com/eggbertx\">Eggbertx</a><br /><br />" +
			"See <a href=\"https://github.com/Eggbertx/QtSphere-IDE/blob/master/LICENSE.txt\">LICENSE.txt</a> for more information."))


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
