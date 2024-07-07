from io import BufferedReader
import struct

from PySide6.QtCore import QLine
from PySide6.QtGui import QImage

from formats.spherefile import SphereFile, FormatException, readSphereString

class ObstructionType:
	NoObstruction = 0
	OldObstruction = 1
	NewObstruction = 2

class Tile:
	image:QImage
	animated:bool
	nextTile:int
	delay:int
	blocked:int
	obstructions:list
	name:str

	def __init__(self, image:QImage = None):
		self.image = image
		self.animated = False
		self.nextTile = -1
		self.delay = 8
		self.blocked = False
		self.obstructions = []
		self.name = ""


class Tileset(SphereFile):
	version:int
	tileWidth:int
	tileHeight:int
	tileBPP:int
	compression:bool
	hasObstructions:bool
	tiles:list[Tile]

	@staticmethod
	def fromReader(reader:BufferedReader, filePath:str):
		rts = Tileset()
		rts.filePath = filePath
		rts._parseFileData(reader)
		return rts

	def __init__(self, filePath:str = None):
		super().__init__(filePath)
		self.tileWidth = 0
		self.tileHeight = 0
		self.tileBPP = 0
		self.compression = False
		self.hasObstructions = False
		self.tiles = []
	
	def _parseFileData(self, file:BufferedReader):
		if file.read(4) != b".rts":
			raise FormatException(self.filePath, "invalid tileset file signature")

		(self.version, numTiles, self.tileWidth, self.tileHeight, self.tileBPP, self.compression,
		self.hasObstructions) = struct.unpack("<5H2?240x", file.read(252))
		if self.version != 1:
			raise FormatException(self.filePath, f"Expected tileset version 1, got {self.version}")

		if self.tileBPP != 32:
			raise FormatException(self.filePath, f"Expected tile BPP to be 32, got {self.tileBPP}")

		if self.compression:
			raise FormatException(self.filePath, "Unexpected compression (not supported by Sphere 1.x maps)")

		numPixels = self.tileWidth * self.tileHeight
		for t in range(numTiles):
			tileBytes = file.read(numPixels * 4)
			self.tiles.append(Tile(QImage(tileBytes, self.tileWidth, self.tileHeight, QImage.Format.Format_RGBA8888)))

		for t in range(numTiles):
			(self.tiles[t].animated, self.tiles[t].nextTile, self.tiles[t].delay,
			self.tiles[t].blocked, numSegments, nameLength) = struct.unpack("<x?2HxB2H20x", file.read(32))
			if nameLength > 0:
				self.tiles[t].name = file.read(nameLength).decode(errors="replace")

			match self.tiles[t].blocked:
				case ObstructionType.NoObstruction:
					pass
				case ObstructionType.OldObstruction:
					self.tiles[t].obstructions = struct.unpack(f"{numPixels}?", file.read(numPixels))
				case ObstructionType.NewObstruction:
					for s in range(numSegments):
						(x1, y1, x2, y2) = struct.unpack("<4H", file.read(8))
						self.tiles[t].obstructions.append(QLine(x1, y1, x2, y2))
				case _:
					raise FormatException(self.filePath, f"Invalid block value for tile ID {t}")

	def _packBytes() -> bytes:
		raise NotImplementedError("Tileset saving not implemented yet")