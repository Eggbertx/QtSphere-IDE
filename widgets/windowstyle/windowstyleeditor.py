from PySide6.QtGui import QAction, QIcon
from PySide6.QtWidgets import QWidget, QToolBar, QMenu, QToolButton

from ui.ui_windowstyleeditor import Ui_WindowStyleEditor

from formats.windowstyle import SphereWindowStyle, WindowStyleBitmap, BackgroundMode
from widgets.sphereeditor import SphereEditorType
from widgets.sphereeditor import SphereEditor

class WindowStyleEditor(SphereEditor):
	ui: Ui_WindowStyleEditor
	toolBar : QToolBar

	@property
	def preview(self):
		return self.ui.windowStylePreview

	@staticmethod
	def openAndAttach(parent: QWidget, filePath:str):
		editor = WindowStyleEditor(parent)
		rws = SphereWindowStyle(filePath)
		rws.open()
		editor.attachWindowStyle(rws)
		return editor

	def __init__(self, parent: QWidget):
		super().__init__(parent, SphereEditorType.WindowStyle)
		self.ui = Ui_WindowStyleEditor()
		self.ui.setupUi(self)
		self.toolBar = QToolBar()
		self.setupToolbar()

	def setupToolbar(self):
		gridAction = QAction(QIcon(":/res/togglegrid.png"), "Toggle Grid", self)
		gridAction.setCheckable(True)
		gridAction.setChecked(True)
		gridAction.toggled.connect(self.preview.setGridVisible)
		self.toolBar.addAction(gridAction)

		propertiesAction = QAction(QIcon.fromTheme("document-properties"), "Window Style Properties", self)
		self.toolBar.addAction(propertiesAction)

		self.toolBar.addSeparator()

		zoomMenuButton = QToolButton(self)
		zoomMenuButton.setText("Zoom")
		zoomMenuButton.setIcon(QIcon.fromTheme("zoom-in"))
		zoomMenuButton.setPopupMode(QToolButton.ToolButtonPopupMode.InstantPopup)

		zoomMenu = QMenu("Set Zoom", self)
		zoomMenu.setIcon(QIcon.fromTheme("zoom-in"))
		zoomMenu.addAction("1x").triggered.connect(lambda: self.preview.setScale(1))
		zoomMenu.addAction("2x").triggered.connect(lambda: self.preview.setScale(2))
		zoomMenu.addAction("4x").triggered.connect(lambda: self.preview.setScale(4))
		zoomMenuButton.setMenu(zoomMenu)

		self.toolBar.addWidget(zoomMenuButton)

		self.ui.verticalLayout.setMenuBar(self.toolBar)

	def attachWindowStyle(self, rws: SphereWindowStyle):
		self.setWindowTitle(f"Window Style Editor - {rws.filePath}")
		self.preview.attachWindowStyle(rws)