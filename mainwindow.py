import sys
from PySide6.QtCore import QCoreApplication, QSettings, Slot, QUrl
from PySide6.QtGui import QIcon, QDesktopServices
from PySide6.QtWidgets import QApplication, QMainWindow, QMessageBox, QComboBox

from qsiproject import QSIProject
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
	startPage: StartPage
	engineSelector: QComboBox
	def __init__(self, settings:QSettings=None, parent=None):
		super().__init__(parent)
		self.ui = Ui_MainWindow()
		self.ui.setupUi(self)
		self.settings = settings
		self.ui.splitter.setStretchFactor(1, 4)
		self.setWindowIcon(QIcon(":/icons/res/icon.png"))
		self.startPage = StartPage(self.ui.openFileTabs)
		self.ui.openFileTabs.addTab(self.startPage, "Start Page")
		self.ui.actionNew_file.setMenu(self.ui.menuNew)
		self.engineSelector = QComboBox(self.ui.mainToolBar)
		self.engineSelector.addItem("neoSphere")
		self.engineSelector.addItem("Sphere 1.x")
		self.ui.mainToolBar.addWidget(self.engineSelector)
		self._setupSettings()
		self._connectActions()
	
	def _setupSettings(self):
		match self.settings.value("whichEngine", "neoSphere"):
			case "Sphere 1.x":
				self.engineSelector.setCurrentText("Sphere 1.x")
				self.ui.actionLegacyConfig.setEnabled(True)
				self.ui.actionConfigure_Engine.setEnabled(True)
			case _:
				self.engineSelector.setCurrentText("neoSphere")
				self.ui.actionLegacyConfig.setEnabled(False)
				self.ui.actionConfigure_Engine.setEnabled(False)
			

	def _connectActions(self):
		self.ui.actionExit.triggered.connect(sys.exit)
		self.ui.actionAbout_Qt.triggered.connect(lambda: QMessageBox.aboutQt(self, "About Qt"))
		self.ui.actionAbout.triggered.connect(lambda: QMessageBox.about(self, "About QtSphere IDE", _ABOUT_STRING))
		self.startPage.projectLoaded.connect(self.loadProject)
		self.startPage.loadProjectAction.triggered.connect(self.loadSelectedProject)
		self.startPage.startGameAction.triggered.connect(self.startGame)
		self.startPage.openProjectDirAction.triggered.connect(self.openSelectedProjectDir)
		self.engineSelector.currentTextChanged.connect(self.engineChanged)


	@Slot(str)
	def engineChanged(self, engine:str):
		match engine:
			case "neoSphere"|"miniSphere":
				self.settings.setValue("whichEngine", "neoSphere")
				self.ui.actionConfigure_Engine.setEnabled(False)
				self.ui.actionLegacyConfig.setEnabled(False)
			case "Sphere 1.x":
				self.settings.setValue("whichEngine", "legacy")
				self.ui.actionConfigure_Engine.setEnabled(True)
				self.ui.actionLegacyConfig.setEnabled(True)

	@Slot()
	def startGame(self):
		game = self.startPage.selectedGame()
		if game is None:
			return
		print("Starting game:", game.name)

	@Slot()
	def loadSelectedProject(self):
		selected = self.startPage.selectedGame()
		if selected is not None:
			self.loadProject(selected)

	@Slot(QSIProject)
	def loadProject(self, project:QSIProject):
		print("Loading project:", project.projectFilePath)


	@Slot()
	def openSelectedProjectDir(self):
		selected = self.startPage.selectedGame()
		project = self.startPage.currentProject if selected is None else selected
		QDesktopServices.openUrl(QUrl.fromLocalFile(project.projectDir))

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
