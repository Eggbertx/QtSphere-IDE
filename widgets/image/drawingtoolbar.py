from enum import Enum, auto

from PySide6.QtCore import Signal, Slot
from PySide6.QtGui import QAction, QIcon
from PySide6.QtWidgets import QMenu, QToolBar, QToolButton, QWidget

class DrawingTool(Enum):
	Pencil = auto()
	Line = auto()
	Rectangle = auto()
	RectangleFilled = auto()
	Fill = auto()
	Select = auto()

class DrawingToolbar(QToolBar):
	pencilMenu:QMenu
	pencil1:QAction
	pencil3:QAction
	pencil5:QAction
	pencilTool:QToolButton
	lineTool:QAction
	fillTool:QAction
	dropperTool:QAction

	pencilSizeChanged: Signal = Signal(int)
	currentToolChanged: Signal = Signal(DrawingTool)

	def __init__(self, parent: QWidget|None = None):
		super().__init__(parent)
		self.pencilMenu = QMenu(self)
		self.pencil1 = self.pencilMenu.addAction(QIcon(":/res/1x1grid.png"), "1x1")
		self.pencil1.triggered.connect(lambda: self.pencilSizeChanged.emit(1))
		self.pencil3 = self.pencilMenu.addAction(QIcon(":/res/3x3grid.png"), "3x3")
		self.pencil3.triggered.connect(lambda: self.pencilSizeChanged.emit(3))
		self.pencil5 = self.pencilMenu.addAction(QIcon(":/res/5x5grid.png"), "5x5")
		self.pencil5.triggered.connect(lambda: self.pencilSizeChanged.emit(5))
		self.pencilMenu.setDefaultAction(self.pencil1)
		self.pencilMenu.triggered.connect(self.setCurrentTool)

		self.pencilTool = QToolButton(self)
		self.pencilTool.setIcon(QIcon(":/res/pencil.png"))
		self.pencilTool.setText("Pencil")
		self.pencilTool.setToolTip("Pencil")
		self.pencilTool.setMenu(self.pencilMenu)
		self.pencilTool.setPopupMode(QToolButton.ToolButtonPopupMode.MenuButtonPopup)
		self.pencilTool.setCheckable(True)
		self.pencilTool.setChecked(True)
		self.addWidget(self.pencilTool)
		self.pencilTool.clicked.connect(lambda: self.setCurrentTool(self.pencilTool))

		self.lineTool = self.addAction(QIcon(":/res/linetool.png"), "Line")
		self.lineTool.setCheckable(True)
		self.rectTool = self.addAction(QIcon(":/res/rectangletool.png"), "Rectangle")
		self.rectTool.setCheckable(True)
		self.fillTool = self.addAction(QIcon(":/res/paintbucket.png"), "Fill layer")
		self.fillTool.setCheckable(True)
		self.dropperTool = self.addAction(QIcon(":/res/dropper.png"), "Select tile")
		self.dropperTool.setCheckable(True)
		self.addSeparator()
		self.gridTool = self.addAction(QIcon(":/res/togglegrid.png"), "Show/Hide grid")
		self.gridTool.setCheckable(True)
		self.actionTriggered.connect(self.setCurrentTool)


	@Slot(QAction)
	def setCurrentTool(self, tool:QAction|QToolButton):
		if tool == self.gridTool:
			return

		self.pencilTool.setChecked(False)
		self.lineTool.setChecked(False)
		self.rectTool.setChecked(False)
		self.fillTool.setChecked(False)
		self.dropperTool.setChecked(False)

		match tool:
			case self.pencil1|self.pencil3|self.pencil5|self.pencilTool:
				self.pencilTool.setChecked(True)
			case self.lineTool:
				self.lineTool.setChecked(True)
			case self.rectTool:
				self.rectTool.setChecked(True)
			case self.fillTool:
				self.fillTool.setChecked(True)
			case self.dropperTool:
				self.dropperTool.setChecked(True)
