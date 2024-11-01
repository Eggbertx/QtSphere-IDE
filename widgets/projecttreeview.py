from os import path

from PySide6.QtCore import Qt, Signal, Slot, QModelIndex
from PySide6.QtGui import QAction, QPixmap, QStandardItemModel, QStandardItem, QContextMenuEvent, QIcon, QGuiApplication
from PySide6.QtWidgets import QTreeView, QFileSystemModel, QMenu, QWidget, QStyle

from qsiproject import QSIProject

class ProjectTreeView(QTreeView):
	fsModel: QFileSystemModel
	contextMenu: QMenu

	showInExplorerAction: QAction
	copyPathAction: QAction
	copyRelativePathAction: QAction

	fileItemActivated: Signal = Signal(str)

	def __init__(self, parent: QWidget|None = None):
		super().__init__(parent)
		self.fsModel = QFileSystemModel(self)
		self.emptyProjectModel = QStandardItemModel(0, 0, self)
		self.emptyProjectModel.appendRow(QStandardItem("<No open project>"))
		self.emptyProjectModel.item(0, 0).setEditable(False)
		self.contextMenu = QMenu(self)
		self.showInExplorerAction = self.addContextMenuAction("Show in File Explorer", QStyle.StandardPixmap.SP_DirIcon)
		self.copyPathAction = self.addContextMenuAction("Copy Path")
		self.copyRelativePathAction = self.addContextMenuAction("Copy Relative Path")
		self.activated.connect(self.onItemActivated)


	def addContextMenuAction(self, text:str, icon:QIcon|QStyle.StandardPixmap|QPixmap = None):
		if icon is None:
			return self.contextMenu.addAction(text)
		if isinstance(icon, QStyle.StandardPixmap):
			return self.contextMenu.addAction(self.style().standardIcon(icon), text)
		return self.contextMenu.addAction(icon, text)


	def updateProject(self, game: QSIProject):
		if game is None:
			self.setModel(self.emptyProjectModel)
			return
		self.fsModel.setRootPath(game.projectDir)
		projectIndex = self.fsModel.index(game.projectDir)
		self.setModel(self.fsModel)
		self.setRootIndex(projectIndex)
		for i in range(self.fsModel.columnCount()):
			if i > 0:
				self.hideColumn(i)


	def contextMenuEvent(self, event: QContextMenuEvent):
		index = self.indexAt(event.pos())
		if self.model() != self.emptyProjectModel and index.isValid():
			action = self.contextMenu.exec(event.globalPos())
			match action:
				case self.copyPathAction:
					QGuiApplication.clipboard().setText(self.fsModel.filePath(index))
				case self.copyRelativePathAction:
					relPath = path.relpath(self.fsModel.filePath(index), self.fsModel.rootPath())
					QGuiApplication.clipboard().setText(relPath)


	@Slot()
	def onItemActivated(self, index: QModelIndex):
		if not self.fsModel.isDir(index):
			self.fileItemActivated.emit(self.fsModel.filePath(index))
