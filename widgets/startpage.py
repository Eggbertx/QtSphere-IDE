from PySide6.QtCore import Qt, Signal, Slot, QPoint, QSettings, QDir, QSize
from PySide6.QtGui import QAction
from PySide6.QtWidgets import QWidget, QMenu, QListWidgetItem

from qsiproject import QSIProject

from ui.ui_startpage import Ui_StartPage

class StartPage(QWidget):
	ui: Ui_StartPage
	baseInfoHTML: str
	gameList: list[QSIProject]
	currentProject: QSIProject
	rightClickMenu: QMenu
	startGameAction: QAction
	loadProjectAction: QAction
	openProjectDirAction: QAction
	refreshGameListAction: QAction
	projectLoaded: Signal = Signal(QSIProject)
	gameStarted: Signal = Signal(str)

	def __init__(self, parent: QWidget = None, windowType: Qt.WindowType = Qt.WindowType.Widget):
		super().__init__(parent, windowType)
		self.ui = Ui_StartPage()
		self.ui.setupUi(self)
		self.baseInfoHTML = self.ui.gameInfoText.toHtml()
		self.ui.splitter.setSizes((250, 100))
		self.setGameInfoText("","","","","")
		self.gameList = []
		self.ui.projectIcons.setContextMenuPolicy(Qt.ContextMenuPolicy.CustomContextMenu)
		self.rightClickMenu = QMenu(self.ui.projectIcons)

		self.currentProject = QSIProject()
		self.startGameAction = self.rightClickMenu.addAction("Start game")
		self.loadProjectAction = self.rightClickMenu.addAction("Load project")
		self.openProjectDirAction = self.rightClickMenu.addAction("Open project directory")
		self.rightClickMenu.addSeparator()
		self.refreshGameListAction = self.rightClickMenu.addAction("Refresh game list")
		self.refreshGameList()

		self.ui.projectIcons.customContextMenuRequested.connect(self.onProjectIconsCustomContextMenuRequested)
		self.ui.projectIcons.itemActivated.connect(self.onProjectIconsItemActivated)
		self.ui.projectIcons.itemSelectionChanged.connect(self.onProjectIconsItemSelectionChanged)
		self.startGameAction.triggered.connect(self.onStartGame)


	@Slot(QPoint)
	def onProjectIconsCustomContextMenuRequested(self, pos:QPoint):
		pass


	@Slot(QListWidgetItem)
	def onProjectIconsItemActivated(self, item: QListWidgetItem):
		self.currentProject = self.gameList[self.ui.projectIcons.indexFromItem(item).row()]
		self.setGameInfoText(
			self.currentProject.name,
			self.currentProject.author,
			self.currentProject.getResolutionString(),
			self.currentProject.projectFilePath,
			self.currentProject.summary)
		self.projectLoaded.emit(self.currentProject)

	@Slot()
	def onProjectIconsItemSelectionChanged(self):
		pass


	@Slot()
	def onStartGame(self):
		self.gameStarted.emit(self.currentProject.buildDir)



	def getGameInfoText(self) -> str:
		self.ui.gameInfoText.toHtml()

	def refreshGameList(self):
		self.gameList.clear()
		self.ui.projectIcons.clear()
		settings = QSettings()
		numProjectDirs = settings.beginReadArray("projectDirs")
		for p in range(numProjectDirs):
			settings.setArrayIndex(p)
			dir = QDir(settings.value("directory"))
			if not dir.exists() or dir.isEmpty():
				continue
			dirs = dir.entryInfoList()
			for projDir in dirs:
				fn = projDir.fileName()
				if fn == "." or fn == "..":
					continue
				project = QSIProject()
				if project.open(projDir.filePath()):
					self.gameList.append(project)
		settings.endArray()

		self.gameList.sort(key=lambda student: student.name)
		for project in self.gameList:
			item = QListWidgetItem(project.getIcon(), project.name)
			item.setTextAlignment(Qt.AlignmentFlag.AlignCenter)
			self.ui.projectIcons.addItem(item)


	def setGameInfoText(self, name:str, author:str, resolution:str, path:str, description:str):
		self.ui.gameInfoText.setHtml(self.baseInfoHTML
			.replace("[PROJECTNAME]", name)
			.replace("[PROJECTAUTHOR]", author)
			.replace("[PROJECTRESOLUTION]", resolution)
			.replace("[PROJECTPATH]", path)
			.replace("[PROJECTDESCRIPTION]", description)
		)

