from PySide6.QtCore import Slot
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
		self.ui.windowStylePreview.activeBitmapChanged.connect(self.windowStyleActiveBitmapChanged)

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
		zoomMenu.addAction("1x").setCheckable(True)
		zoom2 = zoomMenu.addAction("2x")
		zoom2.setCheckable(True)
		zoom2.setChecked(True)
		zoomMenu.addAction("4x").setCheckable(True)
		zoomMenu.triggered.connect(self.zoomMenuTriggered)
		zoomMenuButton.setMenu(zoomMenu)

		self.toolBar.addWidget(zoomMenuButton)

		self.ui.verticalLayout.setMenuBar(self.toolBar)

	def attachWindowStyle(self, rws: SphereWindowStyle):
		self.setWindowTitle(f"Window Style Editor - {rws.filePath}")
		self.preview.attachWindowStyle(rws)
		self.windowStyleActiveBitmapChanged(WindowStyleBitmap.UpperLeft)

	@Slot(int)
	def windowStyleActiveBitmapChanged(self, index: int):
		bitmap = self.preview.windowStyle.bitmaps[index]
		self.ui.bitmapEditor.attachImage(bitmap)

	@Slot(QAction)
	def zoomMenuTriggered(self, action:QAction):
		menu:QMenu = action.parent()
		for item in menu.actions():
			item.setChecked(False)
		match action.text():
			case "1x":
				self.preview.setScale(1)
			case "2x":
				self.preview.setScale(2)
			case "4x":
				self.preview.setScale(4)
		action.setChecked(True)
