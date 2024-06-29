import os
import os.path as path

from PySide6.QtCore import QProcess, QSettings
from PySide6.QtWidgets import QMessageBox, QWidget

from qsiproject import QSIProject, ProjectType

class SphereLauncher:
	settings: QSettings

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

	def __init__(self) -> None:
		self.settings = QSettings()

	def _runSphereProgram(self, isLegacy:bool, program:str, args:list[str], workingDir:str = os.curdir):
		if isLegacy and self.settings.value("legacySphereDir", "") == "":
			raise Exception("Legacy Sphere directory not set")

		if os.name == "nt":
			QProcess.startDetached(program, args, workingDir)
		else:
			if isLegacy:
				QProcess.startDetached("wine", [program] + args, workingDir)
			else:
				QProcess.startDetached(program, args, workingDir)

	def runLegacyConfig(self):
		self._runSphereProgram(True, self.legacySphereConfigPath, [], self.settings.value("legacySphereDir"))
	
	def runLegacyEngine(self, gameDir:str):
		self._runSphereProgram(True, self.legacySphereEnginePath, ["-game", gameDir], gameDir)

	def runNeosphere(self, gameDir:str):
		self._runSphereProgram(False, self.neospherePath, [gameDir], gameDir)

	def launchGame(self, project:QSIProject):
		if self.settings.value("whichEngine", "neosphere") == "neosphere":
			self.runNeosphere(project.buildDir)
		else:
			self.runLegacyEngine(project.buildDir)