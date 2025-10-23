from PySide6.QtWidgets import QWidget

from ui.ui_windowstyleeditor import Ui_WindowStyleEditor

from formats.windowstyle import SphereWindowStyle, WindowStyleBitmap
from widgets.sphereeditor import SphereEditorType
from widgets.sphereeditor import SphereEditor

class WindowStyleEditor(SphereEditor):
	ui: Ui_WindowStyleEditor

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
		self.setupToolbar()

	def setupToolbar(self):
		pass

	def attachWindowStyle(self, rws: SphereWindowStyle):
		self.setWindowTitle(f"Window Style Editor - {rws.filePath}")
		for i in range(9):
			self.preview.setBitmap(i, rws.bitmaps[i])
		# self.preview.setBitmap(WindowStyleBitmap.UpperLeft, rws.bitmaps[WindowStyleBitmap.UpperLeft])