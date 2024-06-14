from PySide6.QtCore import Qt,Signal,Slot,QPoint
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

		self.currentProject = None
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
		pass


	@Slot()
	def onProjectIconsItemSelectionChanged(self):
		pass


	@Slot()
	def onStartGame(self):
		self.gameStarted.emit(self.currentProject.buildDir)



	def getGameInfoText(self) -> str:
		pass


	def loadProject(self, project:QSIProject):
		pass


	def refreshGameList(self):
		pass


	def setGameInfoText(self, name:str, author:str, resolution:str, path:str, description:str):
		pass
