from PySide6.QtCore import QSettings, QRect
from PySide6.QtGui import QColor

class Defaults:
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
		return self.value("geometry")
	
	@geometry.setter
	def geometry(self, geom:QRect):
		self.setValue("geometry", geom)
	
	@geometry.deleter
	def geometry(self):
		self.remove("geometry")

	@property
	def gridColor(self) -> QColor:
		return self.value("gridColor", Defaults.gridColor)
	
	@gridColor.setter
	def gridColor(self, col):
		self.setValue("gridColor", col)

	@gridColor.deleter
	def gridColor(self):
		self.remove("gridColor")

	@property
	def legacySphereDir(self) -> str:
		return self.value("legacySphereDir")

	@legacySphereDir.setter
	def legacySphereDir(self, val:str):
		self.setValue("legacySphereDir", val)

	@property
	def mapCursorColor(self) -> QColor|str:
		return self.value("mapCursorColor", Defaults.mapCursorColor)

	@mapCursorColor.setter
	def mapCursorColor(self, col:QColor|str):
		self.setValue("mapCursorColor", col)
	
	@mapCursorColor.deleter
	def mapCursorColor(self):
		self.remove("mapCursorColor")

	@property
	def maximized(self) -> bool:
		return self.value("maximized", Defaults.maximized)

	@maximized.setter
	def maximized(self, m:bool):
		self.setValue("maximized", m)
	
	@maximized.deleter
	def maximized(self):
		self.remove("maximized")

	@property
	def neosphereDir(self) -> str:
		return self.value("neoSphereDir")

	@neosphereDir.setter
	def neosphereDir(self, dir:str):
		self.setValue("neoSphereDir", dir)
	
	@neosphereDir.deleter
	def neosphereDir(self):
		self.remove("neoSphereDir")

	@property
	def projectDirs(self) -> list[str]:
		return self.getArray("projectDirs", "directory")

	@projectDirs.setter
	def projectDirs(self, dirs:list[str]):
		self.setArray("projectDirs", "directory", dirs)

	@projectDirs.deleter
	def projectDirs(self):
		self.remove("projectDirs")

	@property
	def unrecognizedFileEditor(self) -> str:
		return self.value("unrecognizedFileEditor", Defaults.unrecognizedFileEditor)

	@unrecognizedFileEditor.setter
	def unrecognizedFileEditor(self, editor:str):
		self.setValue("unrecognizedFileEditor", editor)

	@unrecognizedFileEditor.deleter
	def unrecognizedFileEditor(self):
		self.remove("unrecognizedFileEditor")

	@property
	def defaultEngine(self):
		return self.value("defaultEngine")

	@defaultEngine.setter
	def defaultEngine(self, which:str):
		self.setValue("defaultEngine", which)

	@defaultEngine.deleter
	def defaultEngine(self):
		self.remove("defaultEngine")

	@property
	def wineDir(self) -> str:
		return self.value("wineDir")
	
	@wineDir.setter
	def wineDir(self, dir:str):
		self.setValue("wineDir", dir)

	@wineDir.deleter
	def wineDir(self):
		self.remove("wineDir")

	@property
	def theme(self) -> str:
		return self.value("theme", Defaults.theme)

	@theme.setter
	def theme(self, newTheme:str):
		self.setValue("theme", newTheme)

	@theme.deleter
	def theme(self):
		self.remove(Defaults.theme)

	@property
	def defaultZoom(self) -> int:
		return self.value("defaultZoom", Defaults.defaultZoom)

	@defaultZoom.setter
	def defaultZoom(self, z:int):
		self.setValue("defaultZoom", z)

	@defaultZoom.deleter
	def defaultZoom(self):
		self.remove("defaultZoom")

	def __init__(self, filename:str = None) -> None:
		if filename is None:
			super().__init__()
		else:
			super().__init__(filename, QSettings.Format.IniFormat)

	def value(self, key:str, default = None):
		if default is None and hasattr(Defaults, key):
			# no default value passed to function, a constant default exists
			default = getattr(Defaults, key)
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
