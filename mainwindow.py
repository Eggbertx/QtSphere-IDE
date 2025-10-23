import ctypes
from enum import Enum, auto
import os
from os.path import basename
import sys
import traceback

from PySide6.QtCore import QCoreApplication, Qt, Slot, QUrl
from PySide6.QtGui import QCloseEvent, QIcon, QDesktopServices, QImage, QShortcut
from PySide6.QtWidgets import (QApplication, QMainWindow, QMessageBox, QComboBox,
	QFileDialog, QMenu, QToolButton, QWidget, QDialog, QDialogButtonBox, QAbstractButton)

from dialogs.mappropertiesdialog import MapPropertiesDialog
from ui.ui_mainwindow import Ui_MainWindow

from dialogs.errordialog import ErrorDialog
from dialogs.modifiedfilesdialog import ModifiedFilesDialog
from dialogs.newimagedialog import NewImageDialog
from dialogs.newmapdialog import NewMapDialog
from dialogs.projectpropertiesdialog import ProjectPropertiesDialog
from dialogs.settingswindow import SettingsWindow
from formats.spheremap import SphereMap
from qsiproject import QSIProject
from spherelauncher import SphereLauncher
from widgets.image.imageeditor import ImageEditor
from widgets.map.mapeditor import MapEditor
from widgets.sphereeditor import SphereEditor, SphereEditorType
from widgets.spriteset.spriteseteditor import SpritesetEditor
from widgets.startpage import StartPage
from widgets.textedit import TextEdit
from settings import Settings
from widgets.editormenus import EditorMenuProvider
from widgets.windowstyle.windowstyleeditor import WindowStyleEditor

_OPEN_DIALOG_FILTER = (
	"All supported files (*.sgm *.txt *.js *.mjs *.cjs *.rmp *.rss *.rws)",
	"Sphere projects (*.sgm)",
	"Script files (*.js *.mjs, *.cjs, *.ts)",
	"Text files (*.txt *.md)",
	"Images (*.bmp *.gif *.jpeg *.jpg *.png)",
	"Audio files (*.wav *.ogg *.mp3 *.flac *.it *.mod *.s3m *.xm)",
	"Sphere fonts (*.rfn)",
	"Sphere maps (*.rmp)",
	"Spritesets (*.rss)",
	"Windowstyles (*.rws)",
	"All files (*.*)"
)


class FileType(Enum):
	AllSupported = auto()
	Projects = auto()
	Script = auto()
	Text = auto()
	Image = auto()
	Audio = auto()
	SphereFont = auto()
	SphereMap = auto()
	Spriteset = auto()
	WindowStyle = auto()
	All = auto()


class SidebarTab(Enum):
	FileTree = auto()
	TaskList = auto()
	SoundTest = auto()


class MainWindow(QMainWindow):
	startPage: StartPage
	engineSelector: QComboBox
	settingsWindow: SettingsWindow
	loadedProject: QSIProject
	newImageDialog: NewImageDialog
	newMapDialog: NewMapDialog
	launcher: SphereLauncher
	verbose: bool
	projectPropertiesDialog: ProjectPropertiesDialog
	modifiedFilesDialog: ModifiedFilesDialog
	newButton: QToolButton
	openFilePaths: list[str] # each element should correspond to the respective tab index, with Start Page (and any other non-file tabs) being None
	version: str
	activeEditorMenu: QMenu
	menuProvider: EditorMenuProvider

	@property
	def currentTabWidget(self):
		return self.ui.openFileTabs.currentWidget()

	@property
	def currentTabIndex(self):
		return self.ui.openFileTabs.currentIndex()


	def __init__(self, version:str, openPath=None, parent=None, verbose=False):
		super().__init__(parent)
		self.ui = Ui_MainWindow()
		self.ui.setupUi(self)
		self.installEventFilter(self)

		self.version = version
		self.switchSidebarTab(SidebarTab.FileTree)
		self.activeEditorMenu = None
		self.verbose = verbose
		self.settingsWindow = SettingsWindow(self)
		self.ui.splitter.setStretchFactor(1, 4)
		self.startPage = StartPage(self.ui.openFileTabs, printWarnings=self.verbose)
		self.ui.openFileTabs.addTab(self.startPage, "Start Page")
		self.openFilePaths = [None]
		self.menuProvider = EditorMenuProvider(self)

		self.engineSelector = QComboBox(self.ui.mainToolBar)
		self.engineSelector.setMinimumWidth(120)
		self.engineSelector.setToolTip("Sphere engine")
		self.engineSelector.addItem(QIcon(":/res/neosphere.png"), "neoSphere")
		self.engineSelector.addItem(QIcon(":/res/legacyengine.png"),"Sphere 1.x")
		self.ui.mainToolBar.addWidget(self.engineSelector)
		self.engineSelector.setEnabled(False)

		self.newButton = QToolButton()
		self.newButton.setIcon(QIcon.fromTheme("document-new"))
		self.newButton.setText("New file")
		self.newButton.setPopupMode(QToolButton.ToolButtonPopupMode.InstantPopup)
		self.newButton.setMenu(self.ui.menuNew)
		self.ui.mainToolBar.insertWidget(self.ui.actionOpenFile, self.newButton)

		self.loadedProject = None
		self.launcher = SphereLauncher(self)
		self.newImageDialog = NewImageDialog(self)
		self.newMapDialog = NewMapDialog(self)
		self.modifiedFilesDialog = ModifiedFilesDialog(self)
		self.projectPropertiesDialog = ProjectPropertiesDialog(self, self.loadedProject)
		self.ui.treeView.updateProject(self.loadedProject)
		self._setupSettings()
		self._connectActions()
		if openPath is not None:
			self.openProjectFileOrDir(openPath)

		if os.name == "nt":
			# makes it so that our icon shows up correctly in the task bar instead of using pythonw.exe's icon
			ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(f"spherical.QtSphereIDE{self.version}")


	def _setupSettings(self):
		settings = Settings()
		match settings.defaultEngine:
			case "legacy":
				self.engineSelector.setCurrentText("Sphere 1.x")
				self.ui.actionLegacyConfig.setEnabled(True)
			case _:
				self.engineSelector.setCurrentText("neoSphere")
				self.ui.actionLegacyConfig.setEnabled(False)


	def _connectActions(self):
		self.ui.actionExit.triggered.connect(sys.exit)
		self.ui.actionAbout_Qt.triggered.connect(lambda: QMessageBox.aboutQt(self, "About Qt"))
		self.ui.actionAbout.triggered.connect(lambda: QMessageBox.about(self, "About QtSphere IDE", 
			f"QtSphere IDE v{self.version}<br />" +
			"Copyright 2025 by <a href=\"https://github.com/eggbertx\">Eggbertx</a><br /><br />" +
			"See <a href=\"https://github.com/Eggbertx/QtSphere-IDE/blob/master/LICENSE.txt\">LICENSE.txt</a> for more information."))
		self.ui.openFileTabs.tabCloseRequested.connect(self.onTabCloseRequested)
		self.ui.openFileTabs.tabBar().tabMoved.connect(self.onTabMoved)
		self.ui.actionConfigure_QtSphere_IDE.triggered.connect(self.onOpenSettingsWindowTriggered)
		self.settingsWindow.settingsSaved.connect(self.onSettingsSaved)
		self.startPage.projectLoaded.connect(lambda proj: self.loadProject(proj))
		self.startPage.loadProjectAction.triggered.connect(self.onStartPageProjectSelected)
		self.startPage.startGameAction.triggered.connect(self.onStartGameTriggered)
		self.startPage.openProjectDirAction.triggered.connect(self.onOpenSelectedProjectDir)
		self.engineSelector.currentIndexChanged.connect(self.onEngineChanged)
		self.ui.actionClose.triggered.connect(self.closeProject)
		self.ui.actionRefresh.triggered.connect(lambda: self.ui.treeView.updateProject(self.loadedProject))
		self.ui.actionProject_Explorer.triggered.connect(lambda: self.switchSidebarTab(SidebarTab.FileTree))
		self.ui.actionProject_Task_List.triggered.connect(lambda: self.switchSidebarTab(SidebarTab.TaskList))
		self.ui.actionSound_Test.triggered.connect(lambda: self.switchSidebarTab(SidebarTab.SoundTest))
		self.ui.actionSpherical_community.triggered.connect(lambda: QDesktopServices.openUrl("https://spheredev.org/"))
		self.ui.actionQSIGithub.triggered.connect(lambda: QDesktopServices.openUrl("https://github.com/Eggbertx/QtSphere-IDE"))
		self.ui.actionNSGithub.triggered.connect(lambda: QDesktopServices.openUrl("https://github.com/spheredev/neoSphere"))
		self.ui.actionOpen_Game_Directory.triggered.connect(self._openCurrentProjectDir)
		self.ui.treeView.fileItemActivated.connect(self.openFile)
		self.ui.actionOpenFile.triggered.connect(self.onOpenFileTriggered)
		self.ui.actionOpenProject.triggered.connect(self.onOpenProjectTriggered)
		self.ui.openFileTabs.currentChanged.connect(self.onTabChanged)
		self.ui.actionUndo.triggered.connect(self.onUndoTriggered)
		self.ui.actionRedo.triggered.connect(self.onRedoTriggered)
		self.ui.actionCut.triggered.connect(self.onCutTriggered)
		self.ui.actionCopy.triggered.connect(self.onCopyTriggered)
		self.ui.actionPaste.triggered.connect(self.onPasteTriggered)
		self.ui.actionSelect_All.triggered.connect(self.onSelectAllTriggered)
		self.ui.actionLegacyConfig.triggered.connect(self.launcher.runLegacyConfig)
		self.ui.toolbarPlayGame.triggered.connect(self.onGameLaunched)
		self.ui.newProject.triggered.connect(self.projectPropertiesDialog.show)
		self.ui.newImage.triggered.connect(self.newImageDialog.show)
		self.newImageDialog.accepted.connect(self.onNewImageAccepted)
		self.ui.newMap.triggered.connect(self.newMapDialog.show)
		self.newMapDialog.accepted.connect(self.onNewMapAccepted)
		self.ui.actionProject_Properties.triggered.connect(self.projectPropertiesDialog.show)
		self.ui.actionSave.triggered.connect(lambda: self.saveCurrentTab(False))
		self.ui.actionSave_As.triggered.connect(lambda: self.saveCurrentTab(True))
		self.ui.newPlainTextFile.triggered.connect(self.newTextFile)
		self.modifiedFilesDialog.buttonBox.clicked.connect(self.onModifiedFileDialogButtonClicked)

		QShortcut(Qt.Modifier.CTRL | Qt.Key.Key_PageDown, self, self.onNextTabTriggered)
		QShortcut(Qt.Modifier.CTRL | Qt.Key.Key_PageUp, self, self.onPrevTabTriggered)
		QShortcut(Qt.Modifier.CTRL | Qt.Key.Key_Tab, self, self.onNextTabTriggered)
		QShortcut(Qt.Modifier.CTRL | Qt.Modifier.SHIFT | Qt.Key.Key_Tab, self, self.onPrevTabTriggered)
		QShortcut(Qt.Modifier.CTRL | Qt.Key.Key_F4, self, self.onCurrentTabCloseRequested)
		QShortcut(Qt.Modifier.CTRL | Qt.Key.Key_W, self, self.onCurrentTabCloseRequested)


	def _openCurrentProjectDir(self):
		if self.loadedProject is not None:
			QDesktopServices.openUrl(self.loadedProject.projectDir)


	def launchGame(self, game:QSIProject):
		if game is None or game.buildDir is None or game.buildDir == "":
			ErrorDialog.showError(self, "A game does not appear to be loaded", "Error launching game")
			return

		try:		
			self.launcher.launchGame(game)
		except Exception as e:
			ErrorDialog.showError(self, f"Unable to run game: {e}", "Error launching game")


	def newTextFile(self):
		editor = TextEdit(self.ui.openFileTabs)
		editor.setModified(True)
		self.openAndGoToNewEditorWidget(editor, "")


	def openAndGoToNewEditorWidget(self, editor:QWidget, filePath:str):
		self.openFilePaths.append(filePath)
		t = self.ui.openFileTabs.addTab(editor, "<new file>" if filePath == "" else basename(filePath))
		self.ui.openFileTabs.setCurrentIndex(t)


	def updateZoomMenu(self, editorType:SphereEditorType):
		self.ui.menuZoom.setEnabled(editorType is not None and editorType in (SphereEditorType.Map, SphereEditorType.Spriteset, SphereEditorType.Image))


	def setEditorMenu(self, editorType: SphereEditorType, editor:SphereEditor = None):
		if self.activeEditorMenu is not None:
			self.ui.menuBar.removeAction(self.activeEditorMenu.menuAction())
		
		beforeAction = self.ui.menuTools.menuAction()
		match editorType:
			case SphereEditorType.Map:
				self.activeEditorMenu = self.menuProvider.createMapMenu(editor)
				self.ui.menuBar.insertAction(beforeAction, self.activeEditorMenu.menuAction())
			case SphereEditorType.Spriteset:
				self.activeEditorMenu = self.menuProvider.createSpritesetMenu(editor)
				self.ui.menuBar.insertAction(beforeAction, self.activeEditorMenu.menuAction())
			case SphereEditorType.WindowStyle:
				self.activeEditorMenu = self.menuProvider.createWindowStyleMenu(editor)
				self.ui.menuBar.insertAction(beforeAction, self.activeEditorMenu.menuAction())



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
				case ".rws":
					editor = WindowStyleEditor.openAndAttach(self.ui.openFileTabs, filePath)
				case ".txt"|".js"|".cjs"|".mjs"|".ts"|".md"|".sgm":
					editor = TextEdit.openAndAttach(self.ui.openFileTabs, filePath)
				case ".bmp"|".gif"|".jpeg"|".jpg"|".png":
					editor = ImageEditor.openAndAttach(self.ui.openFileTabs, filePath)
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
			errorDialog = QMessageBox(QMessageBox.Icon.Critical, "Error", str(e))
			errorDialog.setInformativeText(traceback.format_exc())
			errorDialog.show()
			raise



	def openProjectFileOrDir(self, openPath:str):
		ext = openPath[openPath.rindex("."):] if openPath.count(".") > 0 else ""
		match ext:
			case ".sgm":
				project = QSIProject()
				if project.open(openPath):
					self.loadProject(project)
				else:
					ErrorDialog.showError(self, "Unable to open Sphere 1.x project file")
			case ".ssproj":
				project = QSIProject()
				if project.open(openPath):
					self.loadProject(project)
				else:
					ErrorDialog.showError(self, "Unable to open Sphere Studio project file")
			case _:
				if os.path.isdir(openPath):
					project = QSIProject()
					if project.open(openPath):
						self.loadProject(project)
					else:
						ErrorDialog.showError(self, "Unable to open project directory")
				else:
					self.openFile(openPath)


	def closeProject(self):
		self.loadedProject = None
		self.ui.treeView.updateProject(None)
		self.setWindowTitle(f"QtSphere IDE {self.version}")
		self.ui.menuProject.setEnabled(False)
		self.engineSelector.setEnabled(False)
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
			errorDialog = QMessageBox(QMessageBox.Icon.Critical, "Error", str(e))
			errorDialog.setInformativeText(traceback.format_exc())
			errorDialog.show()
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
			self.launcher.process.kill()
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


	def loadProject(self, project:QSIProject):
		print("Loading project:", project.projectDir)
		self.setWindowTitle(f"QtSphereIDE {self.version} - {project.name}")
		self.ui.menuProject.setEnabled(True)
		self.engineSelector.setEnabled(True)
		self.ui.actionProject_Properties.setEnabled(True)
		self.loadedProject = project
		self.ui.toolbarPlayGame.setEnabled(True)
		self.newMapDialog.projectPath = project.projectDir
		self.projectPropertiesDialog.project = self.loadedProject
		self.ui.treeView.updateProject(project)
		self.switchSidebarTab(SidebarTab.FileTree)


	#region Slots
	@Slot(QAbstractButton)
	def onModifiedFileDialogButtonClicked(self, btn:QAbstractButton):
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
	def onNewImageAccepted(self):
		image = QImage(self.newImageDialog.imageWidth, self.newImageDialog.imageHeight,
			QImage.Format.Format_RGBA8888)
		color = self.newImageDialog.fillColor
		color.setAlpha(self.newImageDialog.fillOpacity)
		image.fill(self.newImageDialog.fillColor)
		editor = ImageEditor(self.ui.openFileTabs, image)
		self.openAndGoToNewEditorWidget(editor, "")
		editor.modificationChanged.connect(self.onCurrentFileModificationChanged)


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
		
		editor:SphereEditor = self.ui.openFileTabs.currentWidget()
		if hasattr(editor, "editorType"):
			self.setEditorMenu(editor.editorType, editor)
			self.updateZoomMenu(editor.editorType)
		else:
			self.setEditorMenu(None)
			self.updateZoomMenu(SphereEditorType.Other)


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
			ErrorDialog.showError(self, "Invalid tab path. This should not normally happen", "Error")
			return


	@Slot()
	def onUndoTriggered(self):
		if self.currentTabHasUndoRedo():
			self.currentTabWidget.undo()


	def onRedoTriggered(self):
		if self.currentTabHasUndoRedo():
			self.currentTabWidget.redo()


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
		self.engineSelector.setCurrentIndex(1 if settings.defaultEngine == "legacy" else 0)
		QApplication.setStyle(settings.theme)


	@Slot()
	def onOpenFileTriggered(self):
		filePath = self.showOpenFileDialog(FileType.AllSupported,
			self.loadedProject.projectDir if self.loadedProject is not None else ".")
		if filePath is not None:
			self.openFile(filePath)


	@Slot()
	def onOpenProjectTriggered(self):
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
			ErrorDialog.showError(self, "Unable to open project directory")


	@Slot()
	def onOpenSettingsWindowTriggered(self):
		self.settingsWindow.loadSettings()
		self.settingsWindow.show()


	@Slot(int,int)
	def onTabMoved(self, fromIndex:int, toIndex:int):
		fromPath = self.openFilePaths.pop(fromIndex)
		if toIndex < len(self.openFilePaths):
			self.openFilePaths.insert(toIndex, fromPath)
		else:
			self.openFilePaths.append(fromPath)

	@Slot()
	def onCurrentTabCloseRequested(self):
		self.ui.openFileTabs.tabCloseRequested.emit(self.ui.openFileTabs.currentIndex())

	@Slot(int)
	def onTabCloseRequested(self, index:int):
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
	def onEngineChanged(self, index:int):
		settings = Settings()
		match index:
			case 0:
				settings.defaultEngine ="neosphere"
				self.ui.actionLegacyConfig.setEnabled(False)
			case 1:
				settings.defaultEngine ="legacy"
				self.ui.actionLegacyConfig.setEnabled(True)


	@Slot()
	def onStartGameTriggered(self):
		game = self.startPage.selectedGame()
		if game is None:
			return
		self.launchGame(game)


	@Slot()
	def onStartPageProjectSelected(self):
		selected = self.startPage.selectedGame()
		if selected is None:
			self.closeProject()
		else:
			self.loadProject(selected)


	@Slot()
	def onOpenSelectedProjectDir(self):
		selected = self.startPage.selectedGame()
		project = self.startPage.currentProject if selected is None else selected
		QDesktopServices.openUrl(QUrl.fromLocalFile(project.projectDir))
	#endregion
