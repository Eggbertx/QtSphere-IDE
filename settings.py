from enum import Enum

from PySide6.QtCore import QSettings, QRect
from PySide6.QtGui import QColor

class Defaults(Enum):
	geometry = QRect()
	gridColor = QColor.fromString("#FF00FF")
	legacySphereDir = ""
	mapCursorColor = QColor.fromString("#0080ff")
	maximized = True
	neoSphereDir = ""
	projectDirs = []
	unrecognizedFileEditor = "text"
	defaultEngine = "neoSphere"
	wineDir = "/usr/bin"
	theme = "Fusion"
	defaultZoom = 2


class Settings(QSettings):
	@property
	def geometry(self) -> QRect:
		return self.value(Defaults.geometry.name)
	
	@geometry.setter
	def geometry(self, geom:QRect):
		self.setValue(Defaults.geometry.name, geom)
	
	@geometry.deleter
	def geometry(self):
		self.remove(Defaults.geometry.name)

	@property
	def gridColor(self) -> QColor:
		return self.value(Defaults.gridColor.name)
	
	@gridColor.setter
	def gridColor(self, col):
		self.setValue(Defaults.gridColor.name, col)

	@gridColor.deleter
	def gridColor(self):
		self.remove(Defaults.gridColor.name)

	@property
	def legacySphereDir(self) -> str:
		return self.value(Defaults.legacySphereDir.name)

	@legacySphereDir.setter
	def legacySphereDir(self, val:str):
		self.setValue(Defaults.legacySphereDir.name, val)

	@property
	def mapCursorColor(self) -> QColor|str:
		return self.value(Defaults.mapCursorColor.name, Defaults.mapCursorColor.value)

	@mapCursorColor.setter
	def mapCursorColor(self, col:QColor|str):
		self.setValue(Defaults.mapCursorColor.name, col)
	
	@mapCursorColor.deleter
	def mapCursorColor(self):
		self.remove(Defaults.mapCursorColor.name)

	@property
	def maximized(self) -> bool:
		return self.value(Defaults.maximized.name, Defaults.maximized.value)

	@maximized.setter
	def maximized(self, m:bool):
		self.setValue(Defaults.maximized.name, m)
	
	@maximized.deleter
	def maximized(self):
		self.remove(Defaults.maximized.name)

	@property
	def neosphereDir(self) -> str:
		return self.value(Defaults.neoSphereDir.name)

	@neosphereDir.setter
	def neosphereDir(self, dir:str):
		self.setValue(Defaults.neoSphereDir.name, dir)
	
	@neosphereDir.deleter
	def neosphereDir(self):
		self.remove(Defaults.neoSphereDir.name)

	@property
	def projectDirs(self) -> list[str]:
		return self.getArray(Defaults.projectDirs.name, "directory")

	@projectDirs.setter
	def projectDirs(self, dirs:list[str]):
		self.setArray(Defaults.projectDirs.name, "directory", dirs)

	@projectDirs.deleter
	def projectDirs(self):
		self.remove(Defaults.projectDirs.name)

	@property
	def unrecognizedFileEditor(self) -> str:
		return self.value(Defaults.unrecognizedFileEditor.name, Defaults.unrecognizedFileEditor.value)

	@unrecognizedFileEditor.setter
	def unrecognizedFileEditor(self, editor:str):
		self.setValue(Defaults.unrecognizedFileEditor.name, editor)

	@unrecognizedFileEditor.deleter
	def unrecognizedFileEditor(self):
		self.remove(Defaults.unrecognizedFileEditor.name)

	@property
	def defaultEngine(self):
		return self.value(Defaults.defaultEngine.name)

	@defaultEngine.setter
	def defaultEngine(self, which:str):
		self.setValue(Defaults.defaultEngine.name, which)

	@defaultEngine.deleter
	def defaultEngine(self):
		self.remove(Defaults.defaultEngine.name)

	@property
	def wineDir(self) -> str:
		return self.value(Defaults.wineDir.name)
	
	@wineDir.setter
	def wineDir(self, dir:str):
		self.setValue(Defaults.wineDir.name, dir)

	@wineDir.deleter
	def wineDir(self):
		self.remove(Defaults.wineDir.name)

	@property
	def theme(self) -> str:
		return self.value(Defaults.theme.name, Defaults.theme.value)

	@theme.setter
	def theme(self, newTheme:str):
		self.setValue(Defaults.theme.name, newTheme)

	@theme.deleter
	def theme(self):
		self.remove(Defaults.theme)

	@property
	def defaultZoom(self) -> int:
		return self.value(Defaults.defaultZoom.name, Defaults.defaultZoom.value)

	@defaultZoom.setter
	def defaultZoom(self, z:int):
		self.setValue(Defaults.defaultZoom.name, z)

	@defaultZoom.deleter
	def defaultZoom(self):
		self.remove(Defaults.defaultZoom.name)

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
	
	def getArray(self, arrayKey:str|Defaults, indexKey:str):
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
