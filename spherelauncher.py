import os
import os.path as path
import sys

from PySide6.QtCore import QProcess, Slot
from PySide6.QtWidgets import QWidget

from dialogs.errordialog import ErrorDialog
from qsiproject import QSIProject
from settings import Settings

class SphereLauncher:
	settings: Settings
	process:QProcess
	output:str
	parent:QWidget
	sphereName:str
	manuallyStopped:bool

	@property
	def winePath(self):
		return path.join(self.settings.wineDir, "wine")

	@property
	def neospherePath(self):
		return path.join(self.settings.neosphereDir, "neosphere")

	@property
	def ssjPath(self):
		return path.join(self.settings.neosphereDir, "ssj")

	@property
	def cellPath(self):
		return path.join(self.settings.neosphereDir, "cell")

	@property
	def legacySphereEnginePath(self):
		return path.join(self.settings.legacySphereDir, "engine.exe")

	@property
	def legacySphereConfigPath(self):
		return path.join(self.settings.legacySphereDir, "config.exe")


	def __init__(self, parent:QWidget = None) -> None:
		self.parent = parent
		self.output = ""
		self.settings = Settings()
		self.process = QProcess(parent)
		self.process.readyReadStandardOutput.connect(self.onProcessStdout)
		self.process.readyReadStandardError.connect(self.onProcessStderr)
		self.process.errorOccurred.connect(self.onProcessErrorOccured)
		self.process.finished.connect(self.onProcessFinished)
		self.sphereName = "Sphere"
		self.manuallyStopped = False


	def _runSphereProgram(self, isLegacy:bool, program:str, args:list[str], workingDir:str = os.curdir):
		self.manuallyStopped = False
		if isLegacy and self.settings.legacySphereDir == "":
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
		self._runSphereProgram(True, self.legacySphereConfigPath, [], self.settings.legacySphereDir)


	def runLegacyEngine(self, gameDir:str):
		self.sphereName = "Sphere 1.x"
		self._runSphereProgram(True, self.legacySphereEnginePath, ["-game", gameDir], gameDir)


	def runNeosphere(self, gameDir:str):
		self.sphereName = "neoSphere"
		self._runSphereProgram(False, self.neospherePath, [gameDir], gameDir)


	def launchGame(self, project:QSIProject):
		if self.settings.defaultEngine == "neosphere":
			self.runNeosphere(project.buildDir)
		else:
			self.runLegacyEngine(project.buildDir)


	def isRunning(self):
		return self.process.state() != QProcess.ProcessState.NotRunning


	def stopGame(self):
		if self.isRunning():
			self.manuallyStopped = True
			self.process.terminate()


	def showErrorWithOutput(self, error:str, title="Unable to launch game"):
		informativeText = \
			f"Command: {self.process.program()}<br>" + \
			f"Arguments: {self.process.arguments()}"

		if self.output != "":
			informativeText += f"<br>{self.sphereName} output:<br>{self.output}"
		
		ErrorDialog.showError(self.parent, error + "<br>" + informativeText, title)


	@Slot(QProcess.ProcessError)
	def onProcessErrorOccured(self, error:QProcess.ProcessError):
		if not self.manuallyStopped:
			self.showErrorWithOutput(f"An error occured while running {self.sphereName}: {error}", "Error")


	@Slot()
	def onProcessStdout(self):
		data = self.process.readAllStandardOutput().data().decode()
		if self.settings.connectConsole:
			print(data, end="", flush=True)
		self.output += data


	@Slot()
	def onProcessStderr(self):
		data = self.process.readAllStandardError().data().decode()
		if self.settings.connectConsole:
			print(data, end="", flush=True, file=sys.stderr)
		self.output += data


	@Slot(QProcess.ExitStatus)
	def onProcessFinished(self, exitCode: QProcess.ExitStatus|int):
		if exitCode != 0 and not self.manuallyStopped and exitCode != QProcess.ExitStatus.NormalExit:
			self.showErrorWithOutput(f"{self.sphereName} process finished with exit code {exitCode}")
		elif self.manuallyStopped:
			print("Process was manually terminated")
		else:
			print("Process exited normally")
