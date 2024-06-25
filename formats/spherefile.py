from abc import ABC,abstractmethod
from enum import Enum
from io import BufferedReader
from os.path import basename
import struct

class FormatException(Exception):
	filePath:str
	def __init__(self, filePath:str, message: str = None) -> None:
		super().__init__(f"Error parsing {basename(filePath)}" + (": " + message if message is not None else ""))

def readSphereString(file: BufferedReader, pos:int = -1) -> str:
	if pos > -1:
		file.seek(pos)
	strlen = struct.unpack("<h", file.read(2))[0]
	return file.read(strlen).decode().strip("\0")

class SphereFile(ABC):
	filePath: str|None
	def __init__(self, filePath:str = None):
		self.filePath = filePath

	@abstractmethod
	def _parseFileData(self, file: BufferedReader):
		pass

	def openFile(self, filePath:str):
		self.filePath = filePath
		self.open()

	def open(self):
		if self.filePath is None:
			raise Exception("filePath not set")
		with open(self.filePath, "rb") as file:
			self._parseFileData(file)

	@abstractmethod
	def _packBytes() -> bytes:
		pass

	def save(self):
		self.saveAs(self.filePath)

	def saveAs(self, newFile:str):
		ba = self._packBytes()
		self.filePath = newFile
		with open(newFile, "wb") as file:
			ba = self._packBytes()
			file.write(ba)