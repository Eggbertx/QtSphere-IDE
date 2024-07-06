from enum import Enum

from PySide6.QtCore import QSettings, QRect
from PySide6.QtGui import QColor

class Defaults(Enum):
	gridColor = QColor.fromString("#000000")
	mapCursorColor = QColor.fromString("#0080ff")
	whichEngine = "neosphere"
	unrecognizedFileEditor = "text"
	maximized = True
	wineDir = "/usr/bin"

class Keys:
	geometry = "geometry"
	gridColor = "gridColor"
	legacySphereDir = "legacySphereDir"
	mapCursorColor = "mapCursorColor"
	maximized = "maximized"
	neosphereDir = "neosphereDir"
	projectDirs = "projectDirs"
	theme = "theme"
	unrecognizedFileEditor = "unrecognizedFileEditor"
	whichEngine = "whichEngine"
	wineDir = "wineDir"


class Settings(QSettings):
	@property
	def geometry(self) -> QRect:
		return self.value(Keys.geometry)
	
	@geometry.setter
	def geometry(self, geom:QRect):
		self.setValue(Keys.geometry, geom)
	
	@geometry.deleter
	def geometry(self):
		self.remove(Keys.geometry)

	@property
	def gridColor(self) -> QColor:
		return self.value(Keys.gridColor)
	
	@gridColor.setter
	def gridColor(self, col):
		self.setValue(Keys.gridColor, col)

	@gridColor.deleter
	def gridColor(self):
		self.remove(Keys.gridColor)

	@property
	def legacySphereDir(self):
		return self.value(Keys.legacySphereDir)

	@legacySphereDir.setter
	def legacySphereDir(self, val:str):
		self.setValue(Keys.legacySphereDir, val)

	@property
	def mapCursorColor(self):
		return self.value(Keys.mapCursorColor, Defaults.mapCursorColor.value)

	@mapCursorColor.setter
	def mapCursorColor(self, col:QColor):
		self.setValue(Keys.mapCursorColor, col)
	
	@mapCursorColor.deleter
	def mapCursorColor(self):
		self.remove(Keys.mapCursorColor)

	@property
	def maximized(self) -> bool:
		return self.value(Keys.maximized, Defaults.maximized.value)

	@maximized.setter
	def maximized(self, m:bool):
		self.setValue(Keys.maximized, m)
	
	@maximized.deleter
	def maximized(self):
		self.remove(Keys.maximized)

	@property
	def neosphereDir(self) -> str:
		return self.value(Keys.neosphereDir)

	@neosphereDir.setter
	def neosphereDir(self, dir:str):
		self.setValue(Keys.neosphereDir, dir)
	
	@neosphereDir.deleter
	def neosphereDir(self):
		self.remove(Keys.neosphereDir)

	@property
	def projectDirs(self) -> list[str]:
		return self.getArray(Keys.projectDirs, "directory")

	@projectDirs.setter
	def projectDirs(self, dirs:list[str]):
		self.setArray(Keys.projectDirs, "directory", dirs)

	@projectDirs.deleter
	def projectDirs(self):
		self.remove(Keys.projectDirs)

	@property
	def unrecognizedFileEditor(self) -> str:
		return self.value(Keys.unrecognizedFileEditor)

	@unrecognizedFileEditor.setter
	def unrecognizedFileEditor(self, editor:str):
		self.setValue(Keys.unrecognizedFileEditor, editor)

	@unrecognizedFileEditor.deleter
	def unrecognizedFileEditor(self):
		self.remove(Keys.unrecognizedFileEditor)

	@property
	def whichEngine(self):
		return self.value(Keys.whichEngine)

	@whichEngine.setter
	def whichEngine(self, which:str):
		self.setValue(Keys.whichEngine, which)

	@whichEngine.deleter
	def whichEngine(self):
		self.remove(Keys.whichEngine)

	@property
	def wineDir(self):
		return self.value(Keys.wineDir)
	
	@wineDir.setter
	def wineDir(self, dir:str):
		self.setValue(Keys.wineDir, dir)

	@wineDir.deleter
	def wineDir(self):
		self.remove(Keys.wineDir)

	def __init__(self, filename:str = None) -> None:
		if filename is None:
			super().__init__()
		else:
			super().__init__(filename, QSettings.Format.IniFormat)

	def value(self, key:str, default = None):
		if default is None and hasattr(Defaults, key):
			# no default value passed to function, a constant default exists
			default = Defaults[key].value
		return super().value(key, default)
	
	def getArray(self, arrayKey:str|Keys, indexKey:str):
		num = self.beginReadArray(arrayKey)
		arr = []
		for i in range(num):
			self.setArrayIndex(i)
			arr.append(self.value(indexKey))
		self.endArray()
		return arr

	def setArray(self, arrayKey:str, indexKey:str, values:list):
		self.remove(arrayKey)
		self.beginWriteArray(arrayKey)
		for i in range(len(values)):
			self.setArrayIndex(i)
			self.setValue(indexKey, values[i])
		self.endArray()
