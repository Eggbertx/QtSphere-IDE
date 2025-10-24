from PySide6.QtWidgets import QDialog

from ui.ui_windowstylepropertiesdialog import Ui_WindowStylePropertiesDialog

from formats.windowstyle import SphereWindowStyle

class WindowStylePropertiesDialog(QDialog):
	ui: Ui_WindowStylePropertiesDialog

	def __init__(self, parent = None, windowstyle: SphereWindowStyle = None):
		super().__init__(parent)
		self.ui = Ui_WindowStylePropertiesDialog()
		self.ui.setupUi(self)
		self.ui.ulColor_btn.alphaChannel = True
		self.ui.urColor_btn.alphaChannel = True
		self.ui.llColor_btn.alphaChannel = True
		self.ui.lrColor_btn.alphaChannel = True