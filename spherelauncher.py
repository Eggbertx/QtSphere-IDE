import os
import os.path as path

from PySide6.QtCore import QProcess, QSettings, Slot
from PySide6.QtWidgets import QWidget

from dialogs.errordialog import ErrorDialog
from qsiproject import QSIProject, ProjectType

class SphereLauncher:
	settings: QSettings
	process:QProcess
	output:str
	parent:QWidget
	sphereName:str

	@property
	def winePath(self):
		return path.join(self.settings.value("wineDir", ""), "wine")

	@property
	def neospherePath(self):
		return path.join(self.settings.value("neosphereDir", ""), "neosphere")

	@property
	def ssjPath(self):
		return path.join(self.settings.value("neosphereDir", ""), "ssj")

	@property
	def cellPath(self):
		return path.join(self.settings.value("neosphereDir", ""), "cell")

	@property
	def legacySphereEnginePath(self):
		return path.join(self.settings.value("legacySphereDir", ""), "engine.exe")

	@property
	def legacySphereConfigPath(self):
		return path.join(self.settings.value("legacySphereDir", ""), "config.exe")


	def __init__(self, parent:QWidget = None) -> None:
		self.parent = parent
		self.output = ""
		self.settings = QSettings()
		self.process = QProcess(parent)
		self.process.readyReadStandardOutput.connect(self.onProcessStdout)
		self.process.readyReadStandardError.connect(self.onProcessStderr)
		self.process.errorOccurred.connect(lambda e: self.showErrorWithOutput(f"An error occured with the Sphere process: {e}"))
		self.process.finished.connect(self.onProcessFinished)
		self.sphereName = "Sphere"


	def _runSphereProgram(self, isLegacy:bool, program:str, args:list[str], workingDir:str = os.curdir):
		if isLegacy and self.settings.value("legacySphereDir", "") == "":
			raise Exception("Legacy Sphere directory not set")
		self.process.setWorkingDirectory(workingDir)
		if os.name == "nt":
			self.process.setProgram(program)
			self.process.setArguments(args)
		else:
			if isLegacy:
				self.process.setProgram("wine")
				self.process.setArguments([program] + args)
			else:
				self.process.setProgram(program)
				self.process.setArguments(args)
		print("Starting process: ", self.process.program())
		print("Process Arguments: ", self.process.arguments())
		self.process.start(self.process.program(), self.process.arguments())


	def runLegacyConfig(self):
		self.sphereName = "Sphere 1.x config"
		self._runSphereProgram(True, self.legacySphereConfigPath, [], self.settings.value("legacySphereDir"))


	def runLegacyEngine(self, gameDir:str):
		self.sphereName = "Sphere 1.x"
		self._runSphereProgram(True, self.legacySphereEnginePath, ["-game", gameDir], gameDir)


	def runNeosphere(self, gameDir:str):
		self.sphereName = "neoSphere"
		self._runSphereProgram(False, self.neospherePath, [gameDir], gameDir)


	def launchGame(self, project:QSIProject):
		if self.settings.value("defaultEngine", "neosphere") == "neosphere":
			self.runNeosphere(project.buildDir)
		else:
			self.runLegacyEngine(project.buildDir)


	def showErrorWithOutput(self, error:str, title="Unable to launch game"):
		informativeText = \
			f"Command: {self.process.program()}<br>" + \
			f"Arguments: {self.process.arguments()}"

		if self.output != "":
			informativeText += f"<br>{self.sphereName} output:<br>{self.output}"
		
		ErrorDialog.showError(self.parent, error + "<br>" + informativeText, title)



	@Slot(QProcess.ProcessError)
	def onProcessErrorOccured(self, error:QProcess.ProcessError):
		self.showErrorWithOutput(f"An error occured while running {self.sphereName}: {error}", "Error")


	@Slot()
	def onProcessStdout(self):
		data = self.process.readAllStandardOutput().data().decode()
		# print("stdout: ", data)
		self.output += data


	@Slot()
	def onProcessStderr(self):
		data = self.process.readAllStandardError().data().decode()
		# print("stderr: ", data)
		self.output += data


	@Slot(QProcess.ExitStatus)
	def onProcessFinished(self, exitCode: QProcess.ExitStatus|int):
		if exitCode != 0 and exitCode != QProcess.ExitStatus.NormalExit:
			self.showErrorWithOutput(f"{self.sphereName} process finished with exit code {exitCode}")
		else:
			print("Process exited normally")
