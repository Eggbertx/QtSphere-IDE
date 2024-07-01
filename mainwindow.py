from argparse import ArgumentParser
from enum import Enum
from os.path import basename
import sys
import traceback

from PySide6.QtCore import QCoreApplication, QSettings, Slot, QUrl, QModelIndex
from PySide6.QtGui import QIcon, QDesktopServices, QStandardItem, QStandardItemModel
from PySide6.QtWidgets import QApplication, QMainWindow, QMessageBox, QComboBox, QFileSystemModel, QFileDialog, QTextEdit

from ui.ui_mainwindow import Ui_MainWindow

from dialogs.newmapdialog import NewMapDialog
from dialogs.settingswindow import SettingsWindow
from formats.spriteset import SphereSpriteset
from qsiproject import QSIProject
from spherelauncher import SphereLauncher
from widgets.startpage import StartPage
from widgets.spriteset.spriteseteditor import SpritesetEditor

_VERSION = "0.10"
_APPLICATION_NAME = "QtSphere IDE"
_ORG_NAME = "Spherical"
_ABOUT_STRING = f"""QtSphere IDE v{_VERSION}<br />
Copyright 2024 by <a href=\"https://github.com/eggbertx\">Eggbertx</a><br /><br />
See <a href=\"https://github.com/Eggbertx/QtSphere-IDE/blob/master/LICENSE.txt\">LICENSE.txt</a> for more information.
"""
_OPEN_DIALOG_FILTER = (
	"All supported files (*.sgm *.txt *.js *.mjs *.cjs *.rmp *.rss *.rws)",
	"Sphere projects (*.sgm)",
	"Script files (*.js *.mjs, *.cjs)",
	"Text files (*.txt *.md)",
	"Audio files (*.wav *.ogg *.mp3 *.flac *.it *.mod *.s3m *.xm)",
	"Sphere fonts (*.rfn)",
	"Sphere maps (*.rmp)",
	"Spritesets (*.rss)",
	"Windowstyles (*.rws)",
	"All files (*.*)"
)

class FileType(Enum):
	AllSupported = 0
	Projects = 1
	Script = 2
	Text = 3
	Audio = 4
	SphereFont = 5
	SphereMap = 6
	Spriteset = 7
	WindowStyle = 8
	All = 9

class SidebarTab(Enum):
	FileTree = 0
	TaskList = 1
	SoundTest = 2

class MainWindow(QMainWindow):
	settings: QSettings
	startPage: StartPage
	engineSelector: QComboBox
	settingsWindow: SettingsWindow
	fsModel: QFileSystemModel
	emptyProjectModel: QStandardItemModel
	loadedProject: QSIProject
	newMapDialog: NewMapDialog
	launcher: SphereLauncher
	verbose: bool
	def __init__(self, parent=None, settings:QSettings=QSettings(), verbose=False):
		super().__init__(parent)
		self.ui = Ui_MainWindow()
		self.ui.setupUi(self)
		self.verbose = verbose
		self.settings = settings
		self.settingsWindow = SettingsWindow(self)
		self.ui.splitter.setStretchFactor(1, 4)
		self.startPage = StartPage(self.ui.openFileTabs, printWarnings=self.verbose)
		self.ui.openFileTabs.addTab(self.startPage, "Start Page")
		self.ui.actionNew_file.setMenu(self.ui.menuNew)
		self.engineSelector = QComboBox(self.ui.mainToolBar)
		self.engineSelector.addItem("neoSphere")
		self.engineSelector.addItem("Sphere 1.x")
		self.ui.mainToolBar.addWidget(self.engineSelector)
		self.fsModel = QFileSystemModel(self)
		self.emptyProjectModel = QStandardItemModel(0,0,self.ui.treeView)
		self.emptyProjectModel.appendRow(QStandardItem("<No open project>"));
		self.loadedProject = None
		self.launcher = SphereLauncher()
		self.newMapDialog = NewMapDialog(self)
		self._updateTree(self.loadedProject)
		self._setupSettings()
		self._connectActions()
	
	def _setupSettings(self):
		match self.settings.value("whichEngine", "neophere"):
			case "legacy":
				self.engineSelector.setCurrentText("Sphere 1.x")
				self.ui.actionLegacyConfig.setEnabled(True)
				self.ui.actionConfigure_Engine.setEnabled(True)
				self.ui.toolbarPlayGame.setIcon(QIcon(":/icons/res/legacyengine.png"))
			case _:
				self.engineSelector.setCurrentText("neoSphere")
				self.ui.actionLegacyConfig.setEnabled(False)
				self.ui.actionConfigure_Engine.setEnabled(False)
				self.ui.toolbarPlayGame.setIcon(QIcon(":/icons/res/neosphere.png"))
			

	def _connectActions(self):
		self.ui.actionExit.triggered.connect(sys.exit)
		self.ui.actionAbout_Qt.triggered.connect(lambda: QMessageBox.aboutQt(self, "About Qt"))
		self.ui.actionAbout.triggered.connect(lambda: QMessageBox.about(self, "About QtSphere IDE", _ABOUT_STRING))
		self.ui.openFileTabs.tabCloseRequested.connect(self.tabCloseRequested)
		self.ui.actionConfigure_QtSphere_IDE.triggered.connect(self.openSettingsWindow)
		self.settingsWindow.settingsSaved.connect(self.onSettingsSaved)
		self.startPage.projectLoaded.connect(self.loadProject)
		self.startPage.loadProjectAction.triggered.connect(self.loadSelectedProject)
		self.startPage.startGameAction.triggered.connect(self.startGame)
		self.startPage.openProjectDirAction.triggered.connect(self.openSelectedProjectDir)
		self.engineSelector.currentIndexChanged.connect(self.engineChanged)
		self.ui.actionClose.triggered.connect(self.closeProject)
		self.ui.actionRefresh.triggered.connect(self._updateTree)
		self.ui.actionProject_Explorer.triggered.connect(lambda: self.switchSidebarTab(SidebarTab.FileTree))
		self.ui.actionProject_Task_List.triggered.connect(lambda: self.switchSidebarTab(SidebarTab.TaskList))
		self.ui.actionSound_Test.triggered.connect(lambda: self.switchSidebarTab(SidebarTab.SoundTest))
		self.ui.actionSpherical_community.triggered.connect(lambda: QDesktopServices.openUrl("https://spheredev.org/"))
		self.ui.actionQSIGithub.triggered.connect(lambda: QDesktopServices.openUrl("https://github.com/Eggbertx/QtSphere-IDE"))
		self.ui.actionNSGithub.triggered.connect(lambda: QDesktopServices.openUrl("https://github.com/spheredev/neoSphere"))
		self.ui.actionOpen_Game_Directory.triggered.connect(self._openCurrentProjectDir)
		self.ui.treeView.activated.connect(self.treeItemActivated)
		self.ui.toolbarOpenButton.triggered.connect(self.openFileButtonPressed)
		self.ui.actionOpenFile.triggered.connect(self.openFileButtonPressed)
		self.ui.actionOpenProject.triggered.connect(self.openProjectPressed)
		self.ui.openFileTabs.currentChanged.connect(self.onTabChanged)
		self.ui.actionCut.triggered.connect(self.onCutTriggered)
		self.ui.actionCopy.triggered.connect(self.onCopyTriggered)
		self.ui.actionPaste.triggered.connect(self.onPasteTriggered)
		self.ui.actionSelect_All.triggered.connect(self.onSelectAllTriggered)
		self.ui.actionLegacyConfig.triggered.connect(self.launcher.runLegacyConfig)
		self.ui.toolbarPlayGame.triggered.connect(self.launchGame)
		self.ui.newMap.triggered.connect(self.newMapDialog.show)


	def _openCurrentProjectDir(self):
		if self.loadedProject is not None:
			QDesktopServices.openUrl(self.loadedProject.projectDir)

	def _updateTree(self, game: QSIProject):
		if game is None:
			self.ui.treeView.setModel(self.emptyProjectModel)
			return
		self.fsModel.setRootPath(game.projectDir)
		projectIndex = self.fsModel.index(game.projectDir)
		self.ui.treeView.setModel(self.fsModel)
		self.ui.treeView.setRootIndex(projectIndex)
		for i in range(self.fsModel.columnCount()):
			if i > 0:
				self.ui.treeView.hideColumn(i)

	def launchGame(self, game:QSIProject = None):
		if game is None:
			game = self.loadedProject
		if game is None or game.buildDir is None or game.buildDir == "":
			QMessageBox.critical(self, "Error launching game", "A game does not appear to be loaded")
		
		self.launcher.launchGame(game)

	def openFileAsText(self, filePath:str):
		with open(filePath, errors="ignore") as file:
			editor = QTextEdit(self.ui.openFileTabs)
			editor.setTabStopDistance(editor.tabStopDistance()/2)
			editor.setText(file.read())
			t = self.ui.openFileTabs.addTab(editor, basename(filePath))
			self.ui.openFileTabs.setCurrentIndex(t)

	def openFile(self, filePath:str):
		ext = ""
		if filePath.count(".") > 0:
			ext = filePath[filePath.rindex("."):]
		filename = basename(filePath)
		try:
			match ext.lower():
				case ".rss":
					rss = SphereSpriteset(filePath)
					rss.open()
					editor = SpritesetEditor(self.ui.openFileTabs)
					t = self.ui.openFileTabs.addTab(editor, filename)
					editor.attachSpriteset(rss)
					self.ui.openFileTabs.setCurrentIndex(t)
				case ".txt"|".js"|".cjs"|".mjs"|".ts"|".md"|".sgm":
					self.openFileAsText(filePath)
				case _:
					if self.settings.value("unrecognizedFileEditor", "external") == "external":
						QDesktopServices.openUrl(QUrl.fromLocalFile(filePath))
					else:
						self.openFileAsText(filePath)
		except Exception as e:
			QMessageBox.critical(self, "Error", traceback.format_exc())
			raise

	def closeProject(self):
		self.loadedProject = None
		self._updateTree(None)
		self.setWindowTitle(f"QtSphere IDE {_VERSION}")
		self.ui.menuProject.setEnabled(False)
		self.ui.toolbarPlayGame.setEnabled(False)

	def switchSidebarTab(self, tab:SidebarTab):
		match tab:
			case SidebarTab.FileTree:
				self.ui.sideBar.tabBar().setCurrentIndex(0)
			case SidebarTab.TaskList:
				self.ui.sideBar.tabBar().setCurrentIndex(1)
			case SidebarTab.SoundTest:
				self.ui.sideBar.tabBar().setCurrentIndex(2)

	def showOpenFileDialog(self, fileType: FileType = FileType.AllSupported, startDir:str = ".", title:str = "Open File") -> str|None:
		result = QFileDialog.getOpenFileName(self, title, startDir,
			";;".join(_OPEN_DIALOG_FILTER), _OPEN_DIALOG_FILTER[fileType.value])
		return result[0] if len(result) > 0 else None

	def currentTabCanCutCopyPaste(self):
		active = self.ui.openFileTabs.currentWidget()
		return hasattr(active, "cut") and hasattr(active, "copy") and hasattr(active, "paste")

	def currentTabHasUndoRedo(self):
		active = self.ui.openFileTabs.currentWidget()
		return hasattr(active, "undo") and hasattr(active, "redo")

	def currentTabHasSelectAll(self):
		active = self.ui.openFileTabs.currentWidget()
		return hasattr(active, "selectAll")

	@Slot(int)
	def onTabChanged(self, index:int):
		if index < 0:
			return
		hasUndoRedo = self.currentTabHasUndoRedo()
		hasClipboard = self.currentTabCanCutCopyPaste()
		self.ui.actionUndo.setEnabled(hasUndoRedo)
		self.ui.actionRedo.setEnabled(hasUndoRedo)
		self.ui.actionCut.setEnabled(hasClipboard)
		self.ui.actionCopy.setEnabled(hasClipboard)
		self.ui.actionPaste.setEnabled(hasClipboard)
		self.ui.actionSelect_All.setEnabled(self.currentTabHasSelectAll())

	@Slot()
	def onCutTriggered(self):
		if self.currentTabCanCutCopyPaste():
			self.ui.openFileTabs.currentWidget().cut()

	@Slot()
	def onCopyTriggered(self):
		if self.currentTabCanCutCopyPaste():
			self.ui.openFileTabs.currentWidget().copy()

	@Slot()
	def onPasteTriggered(self):
		if self.currentTabCanCutCopyPaste():
			self.ui.openFileTabs.currentWidget().paste()

	@Slot()
	def onSelectAllTriggered(self):
		if self.currentTabHasSelectAll():
			self.ui.openFileTabs.currentWidget().selectAll()

	@Slot()
	def onSettingsSaved(self):
		print("settings saved")
		self.startPage.refreshGameList()
		self.engineSelector.setCurrentIndex(1 if self.settings.value("whichEngine", "neosphere") == "legacy" else 0)

	@Slot()
	def openFileButtonPressed(self):
		filePath = self.showOpenFileDialog(FileType.AllSupported,
			self.loadedProject.projectDir if self.loadedProject is not None else ".")
		if filePath is not None:
			self.openFile(filePath)

	@Slot()
	def openProjectPressed(self):
		numProjectDirs = settings.beginReadArray("projectDirs")
		startDir:str = None
		if numProjectDirs > 0:
			settings.setArrayIndex(0)
			startDir = settings.value("directory")
		settings.endArray()
		projectDir = QFileDialog.getExistingDirectory(self, "Selct project directory", startDir)
		newProject = QSIProject()
		if newProject.open(projectDir):
			self.loadProject(newProject)
		else:
			QMessageBox.critical(self, "Error", "Unable to open project directory")

	@Slot()
	def treeItemActivated(self, index: QModelIndex):
		if not self.fsModel.isDir(index):
			self.openFile(self.fsModel.filePath(index))

	@Slot()
	def openSettingsWindow(self):
		self.settingsWindow.loadSettings()
		self.settingsWindow.show()

	@Slot(int)
	def tabCloseRequested(self, index:int):
		self.ui.openFileTabs.removeTab(index)
		if self.ui.openFileTabs.count() == 0:
			# reopen the start page if there are no more tabs
			self.ui.openFileTabs.addTab(self.startPage, "Start Page")
			self.startPage.refreshGameList()

	@Slot(int)
	def engineChanged(self, index:int):
		match index:
			case 0:
				self.settings.setValue("whichEngine", "neosphere")
				self.ui.actionConfigure_Engine.setEnabled(False)
				self.ui.actionLegacyConfig.setEnabled(False)
				self.ui.toolbarPlayGame.setIcon(QIcon(":/icons/res/neosphere.png"))
			case 1:
				self.settings.setValue("whichEngine", "legacy")
				self.ui.actionConfigure_Engine.setEnabled(True)
				self.ui.actionLegacyConfig.setEnabled(True)
				self.ui.toolbarPlayGame.setIcon(QIcon(":/icons/res/legacyengine.png"))

	@Slot()
	def startGame(self):
		game = self.startPage.selectedGame()
		if game is None:
			return
		self.launchGame(game)

	@Slot()
	def loadSelectedProject(self):
		selected = self.startPage.selectedGame()
		if selected is None:
			self.closeProject()
		else:
			self.loadProject(selected)

	@Slot(QSIProject)
	def loadProject(self, project:QSIProject):
		print("Loading project:", project.projectDir)
		self.setWindowTitle(f"QtSphereIDE {_VERSION} - {project.name}")
		self.ui.menuProject.setEnabled(True)
		self.loadedProject = project
		self.ui.toolbarPlayGame.setEnabled(True)
		self.newMapDialog.projectPath = project.projectDir
		self._updateTree(project)

	@Slot()
	def openSelectedProjectDir(self):
		selected = self.startPage.selectedGame()
		project = self.startPage.currentProject if selected is None else selected
		QDesktopServices.openUrl(QUrl.fromLocalFile(project.projectDir))


if __name__ == "__main__":
	parser = ArgumentParser()
	parser.add_argument("--verbose", "-v",
		action="store_true",
		default=False,
		help="If set, QtSphere IDE will print a warning when it finds a directory with no parseable game file (game.sgm, Cellscript.js, etc)")
	
	args = parser.parse_args()

	QCoreApplication.setApplicationName(_APPLICATION_NAME)
	QCoreApplication.setOrganizationName(_ORG_NAME)
	QCoreApplication.setApplicationVersion(_VERSION)
	app = QApplication(sys.argv)
	app.setStyle("fusion")
	settings = QSettings()
	window = MainWindow(settings=settings, verbose=args.verbose)
	if settings.value("maximized", True):
		window.showMaximized()
	else:
		window.show()
	sys.exit(app.exec())
