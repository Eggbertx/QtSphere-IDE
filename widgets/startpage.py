from PySide6.QtCore import Qt, Signal, Slot, QPoint, QSettings, QDir, QSize
from PySide6.QtGui import QAction
from PySide6.QtWidgets import QWidget, QMenu, QListWidgetItem

from qsiproject import QSIProject

from ui.ui_startpage import Ui_StartPage

class StartPage(QWidget):
	ui: Ui_StartPage
	printWarnings: bool
	baseInfoHTML: str
	gameList: list[QSIProject]
	currentProject: QSIProject
	rightClickMenu: QMenu
	startGameAction: QAction
	loadProjectAction: QAction
	openProjectDirAction: QAction
	refreshGameListAction: QAction
	projectLoaded: Signal = Signal(QSIProject)

	def __init__(self, parent:QWidget = None, windowType:Qt.WindowType = Qt.WindowType.Widget, printWarnings:bool = False):
		super().__init__(parent, windowType)
		self.ui = Ui_StartPage()
		self.ui.setupUi(self)
		self.printWarnings = printWarnings
		self.baseInfoHTML = self.ui.gameInfoText.toHtml()
		self.ui.splitter.setSizes((250, 100))
		self._setGameInfoText("","","","","")
		self.gameList = []
		self.rightClickMenu = QMenu(self.ui.projectIcons)

		self.currentProject = None
		self.startGameAction = self.rightClickMenu.addAction("Start game")
		self.loadProjectAction = self.rightClickMenu.addAction("Load project")
		self.openProjectDirAction = self.rightClickMenu.addAction("Open project directory")
		self.rightClickMenu.addSeparator()
		self.refreshGameListAction = self.rightClickMenu.addAction("Refresh game list")
		self.refreshGameList()

		self.ui.projectIcons.customContextMenuRequested.connect(self.onProjectIconsCustomContextMenuRequested)
		self.ui.projectIcons.itemActivated.connect(self.onProjectIconsItemActivated)
		self.ui.projectIcons.currentItemChanged.connect(self.onProjectIconsCurrentItemChanged)


	def selectedGame(self) -> QSIProject|None:
		selectedIndexes = self.ui.projectIcons.selectedIndexes()
		if len(selectedIndexes) == 0 or selectedIndexes[0].row() >= len(self.gameList):
			return None
		return self.gameList[selectedIndexes[0].row()]


	@Slot(QPoint)
	def onProjectIconsCustomContextMenuRequested(self, pos:QPoint):
		selected = self.ui.projectIcons.itemAt(pos)
		actionsEnabled = selected is not None
		self.startGameAction.setEnabled(actionsEnabled)
		self.loadProjectAction.setEnabled(actionsEnabled)
		self.openProjectDirAction.setEnabled(actionsEnabled)
		choice = self.rightClickMenu.exec(self.ui.projectIcons.mapToGlobal(pos))
		if choice is None:
			return
		match choice:
			case self.loadProjectAction:
				self.ui.projectIcons.itemActivated.emit(selected)


	@Slot(QListWidgetItem)
	def onProjectIconsItemActivated(self, item: QListWidgetItem):
		selectedIndex = self.ui.projectIcons.indexFromItem(item).row()
		self.currentProject = self.gameList[selectedIndex]
		self._setGameInfoText(
			self.currentProject.name,
			self.currentProject.author,
			self.currentProject.getResolutionString(),
			self.currentProject.projectFilePath,
			self.currentProject.summary)
		self.projectLoaded.emit(self.currentProject)

	@Slot(QListWidgetItem, QListWidgetItem)
	def onProjectIconsCurrentItemChanged(self, cur: QListWidgetItem, prev: QListWidgetItem):
		pass

	def getGameInfoText(self) -> str:
		self.ui.gameInfoText.toHtml()

	def refreshGameList(self):
		self.gameList.clear()
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
				if project.open(projDir.filePath(), self.printWarnings):
					self.gameList.append(project)
		settings.endArray()

		self.gameList.sort(key=lambda game: game.name.lower())
		self.ui.projectIcons.clear()
		for project in self.gameList:
			item = QListWidgetItem(project.getIcon(), project.name)
			item.setTextAlignment(Qt.AlignmentFlag.AlignCenter)
			self.ui.projectIcons.addItem(item)


	def _setGameInfoText(self, name:str, author:str, resolution:str, path:str, description:str):
		self.ui.gameInfoText.setHtml(self.baseInfoHTML
			.replace("[PROJECTNAME]", name)
			.replace("[PROJECTAUTHOR]", author)
			.replace("[PROJECTRESOLUTION]", resolution)
			.replace("[PROJECTPATH]", path)
			.replace("[PROJECTDESCRIPTION]", description)
		)

