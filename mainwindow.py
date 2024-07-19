from argparse import ArgumentParser
import ctypes
from enum import Enum
import os
from os.path import basename
import sys
import traceback

from PySide6.QtCore import QCoreApplication, Qt, Slot, QUrl, QModelIndex
from PySide6.QtGui import QCloseEvent, QIcon, QDesktopServices, QStandardItem, QStandardItemModel, QShortcut
from PySide6.QtWidgets import (QApplication, QMainWindow, QMessageBox, QComboBox, QFileSystemModel,
	QFileDialog, QToolButton, QWidget, QDialog, QDialogButtonBox, QAbstractButton)

from ui.ui_mainwindow import Ui_MainWindow

from dialogs.modifiedfilesdialog import ModifiedFilesDialog
from dialogs.newmapdialog import NewMapDialog
from dialogs.projectpropertiesdialog import ProjectPropertiesDialog
from dialogs.settingswindow import SettingsWindow
from formats.spheremap import SphereMap, Tileset
from formats.spherefile import SphereFile
from formats.spriteset import SphereSpriteset
from qsiproject import QSIProject
from spherelauncher import SphereLauncher
from widgets.map.mapeditor import MapEditor
from widgets.sphereeditor import SphereEditor
from widgets.spriteset.spriteseteditor import SpritesetEditor
from widgets.startpage import StartPage
from widgets.textedit import TextEdit
from settings import Settings

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
	"Script files (*.js *.mjs, *.cjs, *.ts)",
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
	startPage: StartPage
	engineSelector: QComboBox
	settingsWindow: SettingsWindow
	fsModel: QFileSystemModel
	emptyProjectModel: QStandardItemModel
	loadedProject: QSIProject
	newMapDialog: NewMapDialog
	launcher: SphereLauncher
	verbose: bool
	projectPropertiesDialog: ProjectPropertiesDialog
	modifiedFilesDialog: ModifiedFilesDialog
	newButton: QToolButton
	openFilePaths: list[str] # each element should correspond to the respective tab index, with Start Page (and any other non-file tabs) being None

	@property
	def currentTabWidget(self):
		return self.ui.openFileTabs.currentWidget()

	@property
	def currentTabIndex(self):
		return self.ui.openFileTabs.currentIndex()

	def __init__(self, parent=None, verbose=False):
		super().__init__(parent)
		self.ui = Ui_MainWindow()
		self.ui.setupUi(self)
		self.installEventFilter(self)

		self.switchSidebarTab(SidebarTab.FileTree)
		self.verbose = verbose
		self.settingsWindow = SettingsWindow(self)
		self.ui.splitter.setStretchFactor(1, 4)
		self.startPage = StartPage(self.ui.openFileTabs, printWarnings=self.verbose)
		self.ui.openFileTabs.addTab(self.startPage, "Start Page")
		self.openFilePaths = [None]

		self.engineSelector = QComboBox(self.ui.mainToolBar)
		self.engineSelector.setMinimumWidth(120)
		self.engineSelector.addItem(QIcon(":/res/neosphere.png"), "neoSphere")
		self.engineSelector.addItem(QIcon(":/res/legacyengine.png"),"Sphere 1.x")
		self.ui.mainToolBar.addWidget(self.engineSelector)

		self.newButton = QToolButton()
		self.newButton.setIcon(QIcon.fromTheme("document-new"))
		self.newButton.setText("New file")
		self.newButton.setPopupMode(QToolButton.ToolButtonPopupMode.InstantPopup)
		self.newButton.setMenu(self.ui.menuNew)
		self.ui.mainToolBar.insertWidget(self.ui.actionOpenFile, self.newButton)

		self.fsModel = QFileSystemModel(self)
		self.emptyProjectModel = QStandardItemModel(0,0,self.ui.treeView)
		self.emptyProjectModel.appendRow(QStandardItem("<No open project>"));
		self.loadedProject = None
		self.launcher = SphereLauncher()
		self.newMapDialog = NewMapDialog(self)
		self.modifiedFilesDialog = ModifiedFilesDialog(self)
		self.projectPropertiesDialog = ProjectPropertiesDialog(self, self.loadedProject)
		self._updateTree(self.loadedProject)
		self._setupSettings()
		self._connectActions()
		if os.name == "nt":
			# makes it so that our icon shows up correctly in the task bar instead of using pythonw.exe's icon
			ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(f"spherical.QtSphereIDE{_VERSION}")


	def _setupSettings(self):
		settings = Settings()
		match settings.whichEngine:
			case "legacy":
				self.engineSelector.setCurrentText("Sphere 1.x")
				self.ui.actionLegacyConfig.setEnabled(True)
			case _:
				self.engineSelector.setCurrentText("neoSphere")
				self.ui.actionLegacyConfig.setEnabled(False)


	def _connectActions(self):
		self.ui.actionExit.triggered.connect(sys.exit)
		self.ui.actionAbout_Qt.triggered.connect(lambda: QMessageBox.aboutQt(self, "About Qt"))
		self.ui.actionAbout.triggered.connect(lambda: QMessageBox.about(self, "About QtSphere IDE", _ABOUT_STRING))
		self.ui.openFileTabs.tabCloseRequested.connect(self.tabCloseRequested)
		self.ui.openFileTabs.tabBar().tabMoved.connect(self.onTabMoved)
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
		self.ui.actionOpenFile.triggered.connect(self.openFileButtonPressed)
		self.ui.actionOpenProject.triggered.connect(self.openProjectPressed)
		self.ui.openFileTabs.currentChanged.connect(self.onTabChanged)
		self.ui.actionCut.triggered.connect(self.onCutTriggered)
		self.ui.actionCopy.triggered.connect(self.onCopyTriggered)
		self.ui.actionPaste.triggered.connect(self.onPasteTriggered)
		self.ui.actionSelect_All.triggered.connect(self.onSelectAllTriggered)
		self.ui.actionLegacyConfig.triggered.connect(self.launcher.runLegacyConfig)
		self.ui.toolbarPlayGame.triggered.connect(self.onGameLaunched)
		self.ui.newMap.triggered.connect(self.newMapDialog.show)
		self.newMapDialog.accepted.connect(self.onNewMapAccepted)
		self.ui.actionProject_Properties.triggered.connect(self.projectPropertiesDialog.show)
		self.ui.actionSave.triggered.connect(lambda: self.saveCurrentTab(False))
		self.ui.actionSave_As.triggered.connect(lambda: self.saveCurrentTab(True))
		self.ui.newPlainTextFile.triggered.connect(self.newTextFile)
		self.modifiedFilesDialog.buttonBox.clicked.connect(self.modifiedFileDialogButtonClicked)

		QShortcut(Qt.Modifier.CTRL | Qt.Key.Key_PageDown, self, self.onNextTabTriggered)
		QShortcut(Qt.Modifier.CTRL | Qt.Key.Key_PageUp, self, self.onPrevTabTriggered)


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

	def launchGame(self, game:QSIProject):
		if game is None or game.buildDir is None or game.buildDir == "":
			QMessageBox.critical(self, "Error launching game", "A game does not appear to be loaded")
			return
		
		self.launcher.launchGame(game)

	def newTextFile(self):
		editor = TextEdit(self.ui.openFileTabs)
		editor.setModified(True)
		self.openAndGoToNewEditorWidget(editor, "")

	def openAndGoToNewEditorWidget(self, editor:QWidget, filePath:str):
		self.openFilePaths.append(filePath)
		t = self.ui.openFileTabs.addTab(editor, "<new file>" if filePath == "" else basename(filePath))
		self.ui.openFileTabs.setCurrentIndex(t)

	def openFile(self, filePath:str):
		ext = ""
		if filePath.count(".") > 0:
			ext = filePath[filePath.rindex("."):]
		try:
			editor:SphereEditor = None
			settings = Settings()
			match ext.lower():
				case ".rmp":
					editor = MapEditor.openAndAttach(self.ui.openFileTabs, filePath)
				case ".rss":
					editor = SpritesetEditor.openAndAttach(self.ui.openFileTabs, filePath)
				case ".rts":
					self.newMapDialog.tilesetPath = filePath
					if self.newMapDialog.exec() == QDialog.DialogCode.Rejected:
						return
					rmp = SphereMap.create(self.newMapDialog.tilesW, self.newMapDialog.tilesH, self.newMapDialog.tilesetPath)
					editor = MapEditor(self.ui.openFileTabs)
					editor.attachMap(rmp)
				case ".txt"|".js"|".cjs"|".mjs"|".ts"|".md"|".sgm":
					editor = TextEdit.openAndAttach(self.ui.openFileTabs, filePath)
				case ".wav"|".ogg"|".mp3"|".flac"|".it"|".mod"|".s3m"|".xm":
					self.ui.soundPlayer.load(filePath)
					self.switchSidebarTab(SidebarTab.SoundTest)
					return
				case _:
					if settings.unrecognizedFileEditor == "external":
						QDesktopServices.openUrl(QUrl.fromLocalFile(filePath))
						return
					else:
						editor = TextEdit.openAndAttach(self.ui.openFileTabs, filePath)
			editor.modificationChanged.connect(self.onCurrentFileModificationChanged)
			self.openAndGoToNewEditorWidget(editor, filePath)
		except Exception as e:
			QMessageBox.critical(self, "Error", traceback.format_exc())
			raise

	def closeProject(self):
		self.loadedProject = None
		self._updateTree(None)
		self.setWindowTitle(f"QtSphere IDE {_VERSION}")
		self.ui.menuProject.setEnabled(False)
		self.ui.actionProject_Properties.setEnabled(False)
		self.ui.toolbarPlayGame.setEnabled(False)
		self.newMapDialog.projectPath = None

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
		return result[0] if len(result) > 0 and result[0] != "" else None

	def showSaveFileDialog(self, fileType: FileType = FileType.AllSupported, startDir:str = ".", title:str = "Save File"):
		result = QFileDialog.getSaveFileName(self, title, startDir,
			";;".join(_OPEN_DIALOG_FILTER), _OPEN_DIALOG_FILTER[fileType.value])
		return result[0] if len(result) > 0 and result[0] != "" else None

	def saveCurrentTab(self, saveAs:bool):
		index = self.currentTabIndex
		filePath = self.openFilePaths[index]
		if saveAs or filePath == "":
			newFilePath = self.showSaveFileDialog()
			if newFilePath is None:
				return # cancelled
			filePath = newFilePath

		try:
			activeWidget = self.currentTabWidget
			if hasattr(activeWidget, "save"):
				activeWidget.save(filePath)
			elif activeWidget is not StartPage:
				raise NotImplementedError(f"Saving has not been implemented yet for this editor ({type(activeWidget)})")
		except Exception as e:
			QMessageBox.critical(self, "Error", traceback.format_exc())
			raise

	def currentTabIsModified(self):
		active = self.currentTabWidget
		return hasattr(active, "isModified") and active.isModified()

	def setCurrentTabSetModified(self, modified:bool):
		active = self.currentTabWidget
		if hasattr(active, "setModified"):
			active.setModified(modified)

	def currentTabCanCutCopyPaste(self):
		active = self.currentTabWidget
		return hasattr(active, "cut") and hasattr(active, "copy") and hasattr(active, "paste")

	def currentTabHasUndoRedo(self):
		active = self.currentTabWidget
		return hasattr(active, "undo") and hasattr(active, "redo")

	def currentTabHasSelectAll(self):
		return hasattr(self.currentTabWidget, "selectAll")

	def showSaveConfirmationDialog(self):
		msgbox = QMessageBox(self)
		msgbox.setStandardButtons(QMessageBox.StandardButton.Save|QMessageBox.StandardButton.Discard|QMessageBox.StandardButton.Cancel)
		QIcon.fromTheme("")
		filename = basename(self.openFilePaths[self.currentTabIndex])
		if filename == "":
			filename = "The current file"
		msgbox.setText(f"{filename} has been modified")
		msgbox.setIcon(QMessageBox.Icon.Question)
		msgbox.setInformativeText("Do you want to save your changes?")
		return msgbox.exec()

	def setStatus(self, status:str, timeout:int = 0):
		if status is None or status == "":
			self.ui.statusBar.clearMessage()
		else:
			self.ui.statusBar.showMessage(status, timeout)
	
	def closeEvent(self, event: QCloseEvent):
		tabs = self.ui.openFileTabs.count()
		self.modifiedFilesDialog.clearPaths()

		for t in range(tabs):
			widget:SphereEditor = self.ui.openFileTabs.widget(t)
			filePath = self.openFilePaths[t]
			if filePath is not None and hasattr(widget, "isModified") and widget.isModified():
				self.modifiedFilesDialog.addPath(filePath if filePath != "" else "<new file>")

		if len(self.modifiedFilesDialog.paths) > 0:
			self.modifiedFilesDialog.show()
			event.ignore()
		else:
			super().closeEvent(event)

	def getExtensionFilterIndex(self, ext:str) -> int:
		match ext:
			case ".js"|".cjs"|".mjs"|".ts":
				return FileType.Script.value
			case ".txt"|".md":
				return FileType.Text.value
			case ".wav"|".ogg"|".mp3"|".flac"|".it"|".mod"|".s3m"|".xm":
				return FileType.Audio.value
			case ".rfn":
				return FileType.SphereFont.value
			case ".rmp":
				return FileType.SphereMap.value
			case ".rss":
				return FileType.Spriteset.value
			case ".rws":
				return FileType.WindowStyle.value
		return FileType.All.value

	def saveAllModified(self):
		tabs = self.ui.openFileTabs.count()
		for t in range(tabs):
			editor:SphereEditor = self.ui.openFileTabs.widget(t)
			filePath = self.openFilePaths[t]
			
			if filePath is None or not hasattr(editor, "isModified") or not editor.isModified():
				continue
		
			ext = filePath[filePath.rindex("."):]
			filter = self.getExtensionFilterIndex(ext)
			saveFilename = self.showSaveFileDialog(filter, "." if self.loadedProject is None else self.loadedProject.projectDir)
			if saveFilename is None:
				# user clicked cancel in save dialog, abort exiting
				return False

			editor.save(saveFilename)
		return True

	#region Slots

	@Slot(QAbstractButton)
	def modifiedFileDialogButtonClicked(self, btn:QAbstractButton):
		match self.modifiedFilesDialog.buttonBox.standardButton(btn):
			case QDialogButtonBox.StandardButton.SaveAll:
				if self.saveAllModified():
					QApplication.exit(0)
			case QDialogButtonBox.StandardButton.Discard:
				self.modifiedFilesDialog.close()
				QApplication.exit(0)
			case QDialogButtonBox.StandardButton.Cancel:
				self.modifiedFilesDialog.close()


	@Slot()
	def onNewMapAccepted(self):
		rmp = SphereMap.create(self.newMapDialog.tilesW, self.newMapDialog.tilesH, self.newMapDialog.tilesetPath)
		editor = MapEditor(self.ui.openFileTabs)
		editor.attachMap(rmp)
		self.openAndGoToNewEditorWidget(editor, "")
		editor.modificationChanged.connect(self.onCurrentFileModificationChanged)

	@Slot()
	def onGameLaunched(self):
		self.launchGame(self.loadedProject)

	@Slot(bool)
	def onCurrentFileModificationChanged(self, modified:bool):
		index = self.currentTabIndex
		tabText = self.ui.openFileTabs.tabBar().tabText(index)
		showsModified = tabText.endswith(" *")

		if modified and not showsModified:
			self.ui.openFileTabs.tabBar().setTabText(index, tabText + " *")
		elif not modified and showsModified:
			self.ui.openFileTabs.tabBar().setTabText(index, tabText[:-2])


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
		index = self.currentTabIndex
		if index < len(self.openFilePaths):
			currentPath = self.openFilePaths[index]
			self.ui.actionSave.setEnabled(currentPath is not None)
			self.ui.actionSave_As.setEnabled(currentPath is not None)

	@Slot()
	def onNextTabTriggered(self):
		next = self.currentTabIndex + 1 if self.currentTabIndex+1 < len(self.openFilePaths) else 0
		self.ui.openFileTabs.setCurrentIndex(next)

	@Slot()
	def onPrevTabTriggered(self):
		prev = self.currentTabIndex - 1 if self.currentTabIndex > 0 else len(self.openFilePaths) - 1
		self.ui.openFileTabs.setCurrentIndex(prev)

	@Slot()
	def onFileSaveTriggered(self):
		currentPath = self.openFilePaths[self.currentTabIndex]
		if currentPath is None:
			QMessageBox.critical(self, "Error", "Invalid tab path. This should not normally happen.")
			return


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
		settings = Settings()
		self.startPage.refreshGameList()
		self.engineSelector.setCurrentIndex(1 if settings.whichEngine == "legacy" else 0)

	@Slot()
	def openFileButtonPressed(self):
		filePath = self.showOpenFileDialog(FileType.AllSupported,
			self.loadedProject.projectDir if self.loadedProject is not None else ".")
		if filePath is not None:
			self.openFile(filePath)

	@Slot()
	def openProjectPressed(self):
		settings = Settings()
		projectDirs = settings.projectDirs
		startDir = None if len(projectDirs) == 0 else projectDirs[0]
		projectDir = QFileDialog.getExistingDirectory(self, "Selct project directory", startDir)
		if projectDir == "" or projectDir is None:
			return
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

	@Slot(int,int)
	def onTabMoved(self, fromIndex:int, toIndex:int):
		fromPath = self.openFilePaths.pop(fromIndex)
		if toIndex < len(self.openFilePaths):
			self.openFilePaths.insert(toIndex, fromPath)
		else:
			self.openFilePaths.append(fromPath)


	@Slot(int)
	def tabCloseRequested(self, index:int):
		if self.currentTabIsModified():
			match self.showSaveConfirmationDialog():
				case QMessageBox.StandardButton.Save:
					self.saveCurrentTab(False)
				case QMessageBox.StandardButton.Discard:
					pass
				case QMessageBox.StandardButton.Cancel:
					return

		widget = self.ui.openFileTabs.widget(index)
		if hasattr(widget, "modificationChanged"):
			widget.modificationChanged.disconnect()

		self.ui.openFileTabs.removeTab(index)
		self.openFilePaths.pop(index)
		if self.ui.openFileTabs.count() == 0:
			# reopen the start page if there are no more tabs
			self.ui.openFileTabs.addTab(self.startPage, "Start Page")
			self.openFilePaths.append(None)
			# self.startPage.refreshGameList()

	@Slot(int)
	def engineChanged(self, index:int):
		settings = Settings()
		match index:
			case 0:
				settings.whichEngine ="neosphere"
				self.ui.actionLegacyConfig.setEnabled(False)
			case 1:
				settings.whichEngine ="legacy"
				self.ui.actionLegacyConfig.setEnabled(True)

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
		self.ui.actionProject_Properties.setEnabled(True)
		self.loadedProject = project
		self.ui.toolbarPlayGame.setEnabled(True)
		self.newMapDialog.projectPath = project.projectDir
		self.projectPropertiesDialog.project = self.loadedProject
		self._updateTree(project)
		self.switchSidebarTab(SidebarTab.FileTree)

	@Slot()
	def openSelectedProjectDir(self):
		selected = self.startPage.selectedGame()
		project = self.startPage.currentProject if selected is None else selected
		QDesktopServices.openUrl(QUrl.fromLocalFile(project.projectDir))
	#endregion

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
	window = MainWindow(verbose=args.verbose)
	settings = Settings()
	if settings.maximized:
		window.showMaximized()
	else:
		window.show()
	sys.exit(app.exec())
