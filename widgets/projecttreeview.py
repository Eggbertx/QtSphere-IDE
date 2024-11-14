import os
from os import path
import shutil

from PySide6.QtCore import Qt, Signal, Slot, QModelIndex, QUrl
from PySide6.QtGui import QAction, QPixmap, QStandardItemModel, QStandardItem, QContextMenuEvent, QIcon, QGuiApplication, QDesktopServices
from PySide6.QtWidgets import QTreeView, QFileSystemModel, QMenu, QWidget, QStyle, QAbstractItemView, QMessageBox

from dialogs.errordialog import ErrorDialog
from qsiproject import QSIProject

class ProjectTreeView(QTreeView):
	fsModel: QFileSystemModel
	contextMenu: QMenu

	openFileAction: QAction
	openFolderAction: QAction
	copyPathAction: QAction
	copyRelativePathAction: QAction
	renameAction: QAction
	deleteAction: QAction

	fileItemActivated: Signal = Signal(str)

	def __init__(self, parent: QWidget|None = None):
		super().__init__(parent)
		self.fsModel = QFileSystemModel(self)
		self.fsModel.setReadOnly(False)
		self.emptyProjectModel = QStandardItemModel(0, 0, self)
		emptyProjectItem = QStandardItem("<No open project>")
		self.emptyProjectModel.appendRow(emptyProjectItem)
		emptyProjectItem.setEditable(False)
		emptyProjectItem.setSelectable(False)
		self.contextMenu = QMenu(self)
		self.openFileAction = self.addContextMenuAction("Open File", QStyle.StandardPixmap.SP_DialogOpenButton)
		self.openFolderAction = self.addContextMenuAction("Open Containing Folder", QStyle.StandardPixmap.SP_DirIcon)
		self.copyPathAction = self.addContextMenuAction("Copy Path")
		self.copyRelativePathAction = self.addContextMenuAction("Copy Relative Path")
		self.contextMenu.addSeparator()
		self.renameAction = self.addContextMenuAction("Rename")
		self.setEditTriggers(QAbstractItemView.EditTrigger.SelectedClicked)
		self.deleteAction = self.addContextMenuAction("Delete", QIcon.ThemeIcon.EditDelete)
		self.activated.connect(self.onItemActivated)


	def addContextMenuAction(self, text:str, icon:QIcon|QStyle.StandardPixmap|QPixmap|QIcon.ThemeIcon = None):
		if icon is None:
			return self.contextMenu.addAction(text)
		if isinstance(icon, QStyle.StandardPixmap):
			return self.contextMenu.addAction(self.style().standardIcon(icon), text)
		if isinstance(icon, QIcon.ThemeIcon):
			return self.contextMenu.addAction(QIcon.fromTheme(icon), text)
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

	def __deleteIndexFile(self, index: QModelIndex):
		filePath = self.fsModel.filePath(index)
		try:
			if self.fsModel.isDir(index):
				shutil.rmtree(filePath)
			else:
				os.remove(filePath)
			self.fsModel.remove(index)
		except Exception as e:
			ErrorDialog.showError(self, f"Unable to delete {filePath}: {e}")

	def contextMenuEvent(self, event: QContextMenuEvent):
		if self.model() == self.emptyProjectModel:
			return
		index = self.indexAt(event.pos())
		if not index.isValid():
			return
		isDir = self.fsModel.isDir(index)
		self.openFileAction.setEnabled(not isDir)
		action = self.contextMenu.exec(event.globalPos())
		filePath = self.fsModel.filePath(index)
		match action:
			case self.openFileAction:
				if not isDir:
					self.fileItemActivated.emit(filePath)
			case self.openFolderAction:
				parentDir = path.split(path.abspath(filePath))[0]
				QDesktopServices.openUrl(QUrl.fromLocalFile(parentDir))
			case self.copyPathAction:
				QGuiApplication.clipboard().setText(filePath)
			case self.copyRelativePathAction:
				relPath = path.relpath(filePath, self.fsModel.rootPath())
				QGuiApplication.clipboard().setText(relPath)
			case self.renameAction:
				self.edit(index)
			case self.deleteAction:
				confirm = QMessageBox.question(self, "Confirm deletion",
					f"Are you sure you want to delete {filePath}?",
					QMessageBox.StandardButton.Yes|QMessageBox.StandardButton.No,
					QMessageBox.StandardButton.No)
				if confirm == QMessageBox.StandardButton.Yes:
					self.__deleteIndexFile(index)


	@Slot(QModelIndex)
	def onItemActivated(self, index: QModelIndex):
		if not self.model() == self.emptyProjectModel and not self.fsModel.isDir(index):
			self.fileItemActivated.emit(self.fsModel.filePath(index))
