import os
from PySide6.QtCore import Qt, Slot
from PySide6.QtWidgets import QDialog, QWidget, QFileDialog

from ui.ui_projectpropertiesdialog import Ui_ProjectPropertiesDialog

from qsiproject import QSIProject

_RESOLUTION_PRESETS = (
	(None, None),  # "Resolution presets" item
	(320, 240),
	(640, 480),
	(800, 600),
	(1024, 768),
	(1280, 720),
	(1366, 768),
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
		self.ui.resolutionCBox.currentIndexChanged.connect(self.onPresetIndexChanged)
		self.ui.pathButton.clicked.connect(self.onPathButtonClicked)


	def show(self):
		self.ui.pathLabel.setEnabled(self.projectDir is None)
		self.ui.pathButton.setEnabled(self.projectDir is None)
		self.ui.pathLineEdit.setText(self.projectDir)
		self.ui.nameLineEdit.setText(self.project.name)
		self.ui.authorLineEdit.setText(self.project.author)
		self.ui.summaryText.setPlainText(self.project.summary)
		self.ui.tabWidget.setCurrentIndex(0)
		if self.project.compiler == "Cell":
			self.ui.tabWidget.setTabEnabled(0, False)
			self.ui.compilerCB.setCurrentIndex(0)
			self.ui.tabWidget.setTabToolTip(0, "See Cellscript.js")
		else:
			self.ui.tabWidget.setTabToolTip(0, None)
			self.ui.tabWidget.setTabEnabled(0, True)
			self.ui.compilerCB.setCurrentIndex(0)

		self.ui.reswLineEdit.setText(str(self.project.width))
		self.ui.reshLineEdit.setText(str(self.project.height))
		self.ui.entryScriptLineEdit.setText(self.project.script)
		if self.projectDir is None:
			self.ui.resolutionCBox.setCurrentIndex(3)
			self.ui.resolutionCBox.setCurrentText("Resolution presets")
		if os.name != "nt":
			self.ui.compilerCB.setItemText(1, "Sphere 1.x (requires WINE in macOS/*nix)")
		self.setWindowTitle("New Project" if self.projectDir is None else "Project Properties")
		return super().show()

#region Slots
	@Slot()
	def onPathButtonClicked(self):
		dir = QFileDialog.getExistingDirectory(self, "Choose path directory", None)
		if dir == "" or dir is None:
			return
		self.ui.pathLineEdit.setText(dir)


	@Slot()
	def onAccept(self):
		self.projectDir = self.ui.pathLineEdit.text()
		self.project.name = self.ui.nameLineEdit.text()
		self.project.author = self.ui.authorLineEdit.text()
		self.project.width = int(self.ui.reswLineEdit.text())
		self.project.height = int(self.ui.reshLineEdit.text())
		self.project.compiler = "Cell" if self.ui.compilerCB.currentIndex() == 0 else "Vanilla"
		self.project.script = self.ui.entryScriptLineEdit.text()
		self.project.summary = self.ui.summaryText.toPlainText()


	@Slot(int)
	def onPresetIndexChanged(self, index):
		if index == 0:
			return
		self.ui.reswLineEdit.setText(str(_RESOLUTION_PRESETS[index][0]))
		self.ui.reshLineEdit.setText(str(_RESOLUTION_PRESETS[index][1]))
#endregion