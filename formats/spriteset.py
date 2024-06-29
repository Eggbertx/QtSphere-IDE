from io import BufferedReader
from os import path
import struct

from .spherefile import SphereFile, FormatException, readSphereString

from PySide6.QtGui import QImage

class SpritesetFrame:
	imageIndex:int
	delay:int

	@staticmethod
	def fromReader(reader: BufferedReader):
		(index, delay) = struct.unpack("<HH4x", reader.read(8))
		return SpritesetFrame(index, delay)

	def __init__(self, index:int, delay:int):
		self.imageIndex = index
		self.delay = delay

class SpritesetDirection:
	name:str
	frames:list[SpritesetFrame]

	@staticmethod
	def fromReader(reader: BufferedReader):
		numFrames = struct.unpack("<H6x", reader.read(8))[0]
		direction = SpritesetDirection(readSphereString(reader), [])
		for f in range(numFrames):
			direction.frames.append(SpritesetFrame.fromReader(reader))
		return direction

	def __init__(self, name:str, frames:list[SpritesetFrame]):
		self.name = name
		self.frames = frames

class SphereSpriteset(SphereFile):
	filePath: str|None
	version:int
	numImages:int
	frameWidth:int
	frameHeight:int
	numDirection:int
	baseX1:int
	baseY1:int
	baseX2:int
	baseY2:int
	images:list[QImage]
	directions:list[SpritesetDirection]
	def __init__(self, filePath: str = None):
		super().__init__(filePath)
		self.version = 0
		self.numImages = 0
		self.frameWidth = 0
		self.frameHeight = 0
		self.numDirection = 0
		self.baseX1 = 0
		self.baseY1 = 0
		self.baseX2 = 0
		self.baseY2 = 0
		self.images = []
		self.directions = []

	def _parseFileData(self, file: BufferedReader):
		super()._parseFileData(file)
		file.seek(0)
		if file.read(4) != b".rss":
			raise FormatException(self.filePath, "invalid file signature")
		(self.version, self.numImages, self.frameWidth, self.frameHeight,
		self.numDirection, self.baseX1, self.baseY1, self.baseX2,
		self.baseY2) = struct.unpack("<9H106x", file.read(124))
		if self.version < 1 or self.version > 3:
			raise FormatException(self.filePath, "invalid version value")
		
		match self.version:
			case 3:
				for i in range(self.numImages):
					numBytes = self.frameWidth * self.frameHeight * 4
					imgBytes = file.read(numBytes)
					self.images.append(QImage(imgBytes, self.frameWidth, self.frameHeight, QImage.Format.Format_RGBA8888))
				for d in range(self.numDirection):
					self.directions.append(SpritesetDirection.fromReader(file))
			case _:
				raise FormatException(self.filePath, "spriteset versions 1 and 2 are not supported yet")
		print("Done parsing")

	def _packBytes() -> bytes:
		return bytes([])