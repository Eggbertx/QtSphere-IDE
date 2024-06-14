from enum import Enum
from io import TextIOWrapper
import re

from PySide6.QtGui import QIcon
from PySide6.QtCore import QDir, QFileInfo
from PySide6.QtWidgets import QMessageBox

_file_filters = ("*.ssproj", "Cellscript.js", "Cellscript.mjs", "Cellscript.cjs", "game.sgm")

class ProjectType(Enum):
	Unknown = 0
	SSProject = 1
	Cellscript_js = 2
	Cellscript_mjs = 3
	Cellscript_cjs = 4
	SGM = 5

class QSIProject:
	name: str
	author: str
	width: int
	height: int
	apiLevel: int
	version: int
	projectType: ProjectType
	projectFilePath: str
	saveID: str
	summary: str
	buildDir: str
	script: str
	projectDir: str
	compiler: str

	def __init__(self):
		self.projectType = ProjectType.Unknown


	def open(self, path: str) -> bool:
		self.projectDir = path
		fileInfo = QFileInfo(path)
		if not fileInfo.exists():
			QMessageBox.critical(None, "Error!", "Project path %s does not exist" % path)
			return False
		
		if fileInfo.isDir():
			infoList = QDir(path).entryInfoList(_file_filters, QDir.Filter.Files|QDir.Filter.NoDotAndDotDot)
			for info in infoList:
				self.projectFilePath = info.filePath()
				filename = info.fileName()
				if info.suffix() == "ssproj":
					self.projectType = ProjectType.SSProject
				elif filename == "Cellscript.js":
					self.projectType = ProjectType.Cellscript_js
					self.compiler = "Cell"
				elif filename == "Cellscript.cjs":
					self.projectType = ProjectType.Cellscript_cjs
					self.compiler = "Cell"
				elif filename == "Cellscript.mjs":
					self.projectType = ProjectType.Cellscript_cjs
					self.compiler = "Cell"
				elif filename == "game.sgm":
					self.projectType = ProjectType.SGM
		else:
			self.projectFilePath = fileInfo.filePath()
			self.projectDir = fileInfo.dir().path()
		
		if self.projectFilePath == "" or self.projectFilePath is None:
			QMessageBox.critical(None, "Error", "Unable to get project info")
			return False

		with open(self.projectFilePath, "r") as projectFile:
			return self._prepareProjectFile(projectFile)


	def getIcon(self) -> QIcon:
		iconInfo = QFileInfo(QDir(self.projectDir), "icon.png");
		if iconInfo.exists():
			return QIcon(iconInfo.canonicalFilePath())
		return QIcon(":/icons/sphere-icon.png")


	def getResolutionString(self) -> str:
		return "%dx%d" % (self.width, self.height)


	def _getCellscriptStringValue(self, cellscriptStr:str, key:str, defaultValue:str = "") -> str:
		matches: re.Match = re.match(r".*(" + key + r"):\s*['\"](.*)['\"].*", cellscriptStr, re.DOTALL|re.MULTILINE)
		if matches is None:
			return defaultValue
		return matches.group(1)


	def _getCellscriptIntValue(self, cellscriptStr:str, key:str, defaultValue:int = 0) -> int:
		matches: re.Match = re.match(r".*(" + key + r")\s*:\s*(\d+).*", cellscriptStr, re.DOTALL|re.MULTILINE)
		if matches is None:
			return defaultValue
		value:str = matches.group(1)
		return int(value) if value.isdigit() else defaultValue


	def _prepareProjectFile(self, projectFile:TextIOWrapper) -> bool:
		match self.projectType:
			case ProjectType.SSProject:
				return self._readSSProj(projectFile)
			case ProjectType.Cellscript_js | ProjectType.Cellscript_mjs | ProjectType.Cellscript_cjs:
				return self._readCellscript(projectFile)
			case ProjectType.SGM:
				return self._readSGM(projectFile)


	def _readSSProj(self, projectFile:TextIOWrapper) -> bool:
		lines = projectFile.readlines()
		for line in lines:
			if line == "" or line.find("=") == -1:
				continue
			parts = line.split("=")
			match parts[0]:
				case "author":
					self.author = parts[1]
				case "buildDir":
					self.buildDir = QDir(self.projectDir).absoluteFilePath(parts[1])
				case "compiler":
					self.compiler = parts[1]
				case "description":
					self.summary = parts[1]
				case "mainScript":
					self.script = parts[1]
				case "name":
					self.name = parts[1]
				case "screenHeight":
					self.height = int(parts[1])
				case "screenWidth":
					self.width = int(parts[1])


	def _readCellscript(self, projectFile:TextIOWrapper) -> bool:
		scriptStr = projectFile.read()
		self.name = self._getCellscriptStringValue(scriptStr, "name")
		self.author = self._getCellscriptStringValue(scriptStr, "author")
		self.version = self._getCellscriptIntValue(scriptStr, "version", 1)
		self.apiLevel = self._getCellscriptIntValue(scriptStr, "apiLevel", 1)
		self.saveID = self._getCellscriptStringValue(scriptStr, "saveID")
		self.summary = self._getCellscriptStringValue(scriptStr, "summary")
		self.script = self._getCellscriptStringValue(scriptStr, "main")
		resolution = self._getCellscriptStringValue(scriptStr, "resolution")
		resolutionArr = resolution.split("x")

		if len(resolutionArr) != 2:
			# expects format <width>x<height>, missing x or more than one x
			QMessageBox.critical(None, "Error", f"Project {self.name} has an invalid resolution string: {resolution}")
			return False
		
		self.width = int(resolutionArr[0]) if resolutionArr[0].isdigit() else -1
		self.height = int(resolutionArr[1]) if resolutionArr[1].isdigit() else -1
		
		if self.width < 1 or self.height < 1:
			# width or height are an invalid number
			QMessageBox.critical(None, "Error", f"Project {self.name} has an invalid resolution string: {resolution}")
			return False

		self.compiler = "Cell"
		self.buildDir = QFileInfo(self.projectFilePath).dir().absoluteFilePath("dist/")
		return True


	def _readSGM(self, projectFile:TextIOWrapper) -> bool:
		lines = projectFile.readlines()
		for line in lines:
			if line == "":
				continue
			parts = line.split("=", 1)
			match parts[0]:
				case "name":
					self.name = parts[1]
				case "author":
					self.author = parts[1]
				case "description":
					self.summary = parts[1]
				case "screen_width":
					self.width = int(parts[1])
				case "screen_height":
					self.height = int(parts[1])
				case "script":
					self.script = parts[1]
		return True
