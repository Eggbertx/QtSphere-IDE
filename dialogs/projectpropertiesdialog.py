from PySide6.QtCore import Qt, Slot
from PySide6.QtWidgets import QDialog, QWidget

from ui.ui_projectpropertiesdialog import Ui_ProjectPropertiesDialog

from qsiproject import QSIProject

_RESOLUTION_PRESETS = (
	(None, None),  # "Resolution presets" item
	(320, 240),
	(640, 480),
	(800, 600),
	(1280, 720),
	(1024, 768),
	(1920, 1080),
)

class ProjectPropertiesDialog(QDialog):
	ui: Ui_ProjectPropertiesDialog
	_project: QSIProject

	@property
	def project(self):
		if self._project is None:
			self._project = QSIProject()
		return self._project

	@project.setter
	def project(self, project:QSIProject):
		self._project = project

	@project.deleter
	def project(self):
		del self._project

	@property
	def projectDir(self):
		return None if self.project.projectDir == "" else self.project.projectDir

	@projectDir.setter
	def projectDir(self, v:str):
		self.project.projectDir = v

	def __init__(self, parent: QWidget | None = None, project: QSIProject = None) -> None:
		super().__init__(parent, Qt.WindowType.Dialog)
		self.ui = Ui_ProjectPropertiesDialog()
		self.ui.setupUi(self)
		self._project = project
		self.accepted.connect(self.onAccept)

	def show(self):
		self.ui.pathLabel.setEnabled(self.projectDir is None)
		self.ui.pathButton.setEnabled(self.projectDir is None)
		self.ui.pathLineEdit.setText(self.projectDir)
		self.ui.nameLineEdit.setText(self.project.name)
		self.ui.authorLineEdit.setText(self.project.author)
		self.setWindowTitle("New Project" if self.projectDir is None else "Project Properties")
		return super().show()

	@Slot()
	def onAccept(self):
		pass

	@Slot(int)
	def onPresetIndexChanged(self, index):
		if index == 0:
			return
		self.ui.reswLineEdit.setText(_RESOLUTION_PRESETS[index])
		self.ui.reshLineEdit.setText(_RESOLUTION_PRESETS[index])