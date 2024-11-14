from PySide6.QtCore import QSize
from PySide6.QtWidgets import QErrorMessage, QWidget, QLabel, QStyle, QTextEdit


class ErrorDialog(QErrorMessage):
	@staticmethod
	def showErrorWithType(parent:QWidget, error:str|Exception, type:str, title:str="Error"):
		msgBox = ErrorDialog(parent)
		msgBox.setWindowTitle(title)
		msgBox.showMessage(error, type)


	@staticmethod
	def showError(parent:QWidget, error:str|Exception, title:str="Error"):
		msgBox = ErrorDialog(parent, True)
		msgBox.setWindowTitle(title)
		msgBox.showMessage(error)


	def __init__(self, parent: QWidget, withType=False):
		super().__init__(parent)
		
		children = self.children()

		children[2].setVisible(withType)

		# Modify the dialog's icon label, since it defaults to SP_MessageBoxInformation
		iconLabel:QLabel = children[0]
		criticalIcon = self.style().standardIcon(QStyle.StandardPixmap.SP_MessageBoxCritical)
		iconSize = self.style().pixelMetric(QStyle.PixelMetric.PM_MessageBoxIconSize, None, self)
		iconLabel.setPixmap(criticalIcon.pixmap(QSize(iconSize, iconSize), self.devicePixelRatio()))

		self.setMinimumSize(500, 300)