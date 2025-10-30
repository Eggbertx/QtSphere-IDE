from PySide6.QtWidgets import QDialog

from ui.ui_windowstylepropertiesdialog import Ui_WindowStylePropertiesDialog

from formats.windowstyle import SphereWindowStyle, BackgroundMode, EdgeOffset, GradientCorner

class WindowStylePropertiesDialog(QDialog):
	ui: Ui_WindowStylePropertiesDialog
	windowStyle: SphereWindowStyle

	@property
	def backgroundMode(self):
		match self.ui.backgroundMode_combo.currentIndex():
			case 0:
				return BackgroundMode.Tiled
			case 1:
				return BackgroundMode.Stretched
			case 2:
				return BackgroundMode.Gradient
			case 3:
				return BackgroundMode.TiledWithGradient
			case 4:
				return BackgroundMode.StretchedWithGradient


	def __init__(self, parent = None, windowstyle: SphereWindowStyle = None):
		super().__init__(parent)
		self.ui = Ui_WindowStylePropertiesDialog()
		self.ui.setupUi(self)
		self.ui.ulColor_btn.alphaChannel = True
		self.ui.urColor_btn.alphaChannel = True
		self.ui.llColor_btn.alphaChannel = True
		self.ui.lrColor_btn.alphaChannel = True
		self.windowStyle = windowstyle
		self.updateUI()


	def updateUI(self):
		if self.windowStyle is None:
			return

		self.ui.backgroundMode_combo.setCurrentIndex(self.windowStyle.backgroundMode)
		self.ui.leftOffset_num.setValue(self.windowStyle.edgeOffsets[EdgeOffset.Left])
		self.ui.topOffset_num.setValue(self.windowStyle.edgeOffsets[EdgeOffset.Top])
		self.ui.rightOffset_num.setValue(self.windowStyle.edgeOffsets[EdgeOffset.Right])
		self.ui.bottomOffset_num.setValue(self.windowStyle.edgeOffsets[EdgeOffset.Bottom])
		self.ui.ulColor_btn.setColor(self.windowStyle.cornerColors[GradientCorner.UpperLeft])
		self.ui.urColor_btn.setColor(self.windowStyle.cornerColors[GradientCorner.UpperRight])
		self.ui.llColor_btn.setColor(self.windowStyle.cornerColors[GradientCorner.LowerLeft])
		self.ui.lrColor_btn.setColor(self.windowStyle.cornerColors[GradientCorner.LowerRight])
