from io import BufferedReader
from os import path
import struct

from PySide6.QtCore import QSize
from PySide6.QtGui import QImage, QColor

from formats.spherefile import SphereFile, FormatException, readSphereString
from formats.tileset import Tileset, Tile

#region Enums

class MapDirections:
	North = 0
	Northeast = 1
	East = 2
	Southeast = 3
	South = 4
	Southwest = 5
	West = 6
	Northwest = 7

class MapString:
	TilesetFile = 0
	MusicFile = 1
	ScriptFile = 2
	EntryScript = 3
	ExitScript = 4
	NorthScript = 5
	EastScript = 6
	SouthScript = 7
	WestScript = 8

class EntityType:
	Person = 1
	Trigger = 2

class EntityScriptType:
	OnCreate = 0
	OnDestroy = 1
	OnActivateTouch = 2
	OnActivateTalk = 3
	GenerateCommands = 4

#endregion

#region Map components

class ObstructionSegment:
	x1:int
	y1:int
	x2:int
	y2:int
	def __init__(self, x1 = 0, y1 = 0, x2 = 0, y2 = 0):
		self.x1 = x1
		self.y1 = y1
		self.x2 = x2
		self.y2 = y2


class MapLayer:
	@property
	def visible(self):
		return self.flags & 1 == 0
	
	@visible.setter
	def visible(self, vis:bool):
		self.flags = self.flags & (~1) if vis else self.flags | 1

	@property
	def hasParallax(self):
		return self.flags & 2 == 2

	width:int
	height:int
	flags:int
	parallaxX:float
	parallaxY:float
	scrollingX:float
	scrollingY:float
	numSegments:int
	reflective:bool
	name:str
	tiles:list[int]
	segments:list[ObstructionSegment]

	@staticmethod
	def fromReader(reader:BufferedReader):
		layer = MapLayer()
		(layer.width, layer.height, layer.flags, layer.parallaxX, layer.parallaxY, layer.scrollingX,
		layer.scrollingY, layer.numSegments, layer.reflective) = struct.unpack("<3H4fL?3x", reader.read(30))
		layer.name = readSphereString(reader)
		layer.tiles = list(struct.unpack(f"<{layer.width*layer.height}H", reader.read(layer.width*layer.height*2)))
		layer.segments = []
		for s in range(layer.numSegments):
			segment = ObstructionSegment()
			(segment.x1, segment.y1, segment.x2, segment.y2) = struct.unpack("<4L", reader.read(16))
			layer.segments.append(segment)
		return layer

	def __init__(self) -> None:
		self.width = 0
		self.height = 0
		self.flags = 0
		self.parallaxX = 1.0
		self.parallaxY = 1.0
		self.scrollingX = 0.0
		self.scrollingY = 0.0
		self.numSegments = 0
		self.reflective = False
		self.name = ""
		self.tiles = []
		self.segments = []


	def resize(self, newWidth:int, newHeight:int):
		if newWidth == self.width and newHeight == self.height:
			return

		newTiles = [0] * newWidth * newHeight
		for y in range(min(self.height, newHeight)):
			for x in range(min(self.width, newWidth)):
				newTiles[x + y * newWidth] = self.tiles[x + y * self.width]

		self.width = newWidth
		self.height = newHeight
		self.tiles = newTiles


class MapEntity:
	mapX:int
	mapY:int
	layer:int
	type:int

	@staticmethod
	def fromReader(reader:BufferedReader, filePath:str):
		(mapX, mapY, layer, type) = struct.unpack("<4H8x", reader.read(16))
		match type:
			case 1:
				entity = MapPerson(mapX, mapY, layer, type)
				entity.name = readSphereString(reader)
				entity.spritesetFilename = readSphereString(reader)
				numScripts = struct.unpack("<H", reader.read(2))[0]
				if numScripts != 5:
					raise FormatException(filePath, f"Expected 5 scripts, in entity at ({mapX},{mapY}) got {numScripts}")
				entity.scripts = []
				for s in range(numScripts):
					entity.scripts.append(readSphereString(reader))
				reader.read(16)
				return entity
			case 2:
				entity = MapTrigger(mapX, mapY, layer, type)
				entity.script = readSphereString(reader)
				return entity
			case _:
				raise FormatException(filePath, f"Invalid entity type {type}")

	def __init__(self, mapX:int, mapY:int, layer:int, type:int):
		self.mapX = mapX
		self.mapY = mapY
		self.layer = layer
		self.type = type


class MapPerson(MapEntity):
	name:str
	spritesetFilename:str
	scripts:list[str]


class MapTrigger(MapEntity):
	script:str


class MapZone:
	x1:int
	y1:int
	x2:int
	y2:int
	layer:int
	reactivateInNumSteps:int
	script:str

	@staticmethod
	def fromReader(reader: BufferedReader):
		zone = MapZone()
		(zone.x1,zone.y1,zone.x2,zone.y2,zone.layer,zone.reactivateInNumSteps) = struct.unpack("<6H4x", reader.read(16))
		zone.script = readSphereString(reader)
		return zone


	def __init__(self, x1:int = 0, y1:int = 0, x2:int = 0, y2:int = 0, layer:int = 0, reactivateInNumSteps:int = 8, script:str = ""):
		self.x1 = x1
		self.y1 = y1
		self.x2 = x2
		self.y2 = y2
		self.layer = layer
		self.reactivateInNumSteps = reactivateInNumSteps
		self.script = script

#endregion

class SphereMap(SphereFile):
	startX: int
	startY: int
	startLayer: int
	startDirection: MapDirections
	tileset: Tileset

	strings:list[str]
	layers:list[MapLayer]
	entities:list[MapEntity]
	zones:list[MapZone]
	repeating:bool

	@property
	def tilesetFile(self):
		if len(self.strings) < MapString.TilesetFile:
			raise FormatException(self.filePath, "Missing tileset string data")
		return self.strings[MapString.TilesetFile]


	@tilesetFile.setter
	def tilesetFile(self, value:str):
		if len(self.strings) < MapString.TilesetFile:
			self.strings.append("")
		self.strings[MapString.TilesetFile] = value


	@staticmethod
	def create(width:int, height:int, tileset:str = None):
		rts = Tileset()
		if tileset != "" and tileset is not None:
			rts.openFile(tileset)
		else:
			rts.tileBPP = 32
			rts.tileWidth = 16
			rts.tileHeight = 16
			img = QImage(16, 16, QImage.Format.Format_ARGB32)
			img.fill(QColor("#000000"))
			rts.tiles.append(Tile(img))

		layer = MapLayer()
		layer.name = "Base"
		layer.width = width
		layer.height = height
		layer.tiles = [0] * width * height

		rmp = SphereMap()
		rmp.tileset = rts
		rmp.layers.append(layer)
		rmp.strings = [""] * 9
		rmp.strings[MapString.TilesetFile] = rts.filePath or ""
		return rmp


	def __init__(self, filePath: str = None):
		super().__init__(filePath)
		self.strings = []
		self.layers = []
		self.entities = []
		self.zones = []
		self.tileset = None
		self.startX = 0
		self.startY = 0
		self.repeating = False


	def open(self):
		super().open()
		if self.tilesetFile != "":
			with open(self.tilesetPath(), "rb") as file:
				self.tileset = Tileset.fromReader(file, self.tilesetFile)


	def _parseFileData(self, file: BufferedReader):
		super()._parseFileData(file)
		file.seek(0)
		if file.read(4) != b".rmp":
			raise FormatException(self.filePath, f"invalid file signature")
		
		(version, numLayers, numEntities, self.startX, self.startY,
		self.startLayer, self.startDirection, numStrings, numZones, self.repeating) = struct.unpack("<HxBx3H2B2H?234x", file.read(252))
		if version != 1:
			raise FormatException(self.filePath, f"invalid map file version (must be 1, got {version})")

		if numStrings != 9:
			raise FormatException(self.filePath, f"invalid number of strings in map, (expected 9, got {numStrings})")

		for s in range(numStrings):
			self.strings.append(readSphereString(file))
	
		for l in range(numLayers):
			self.layers.append(MapLayer.fromReader(file))

		for e in range(numEntities):
			entity = MapEntity.fromReader(file, self.filePath)
			if entity.layer < 0 or entity.layer >= numLayers:
				raise FormatException(self.filePath, f"Invalid entity layer {entity.layer}, expected layer between 0 and {numLayers}")
			self.entities.append(entity)

		for z in range(numZones):
			zone = MapZone.fromReader(file)
			if zone.layer < 0 or zone.layer >= numLayers:
				raise FormatException(self.filePath, f"Invalid zone layer {zone.layer}, expected layer between 0 and {numLayers}")
			self.zones.append(zone)

		if self.tilesetFile == "":
			self.tileset = Tileset.fromReader(file, "")


	def tilesetPath(self):
		mapDir = "." if self.filePath is None else path.dirname(self.filePath)
		return path.normpath(path.join(mapDir, self.tilesetFile)).replace("\\", "/")


	def _packBytes() -> bytes:
		raise NotImplementedError("Map saving not implemented yet")


	def getString(self, string:MapString):
		if string < 0 or string >= len(self.strings):
			return None
		return self.strings[string]


	def setString(self, string:MapString, value:str):
		if string < 0:
			raise IndexError(f"Invalid map string index {string}")
		if string >= len(self.strings):
			self.strings.extend([""] * (string - len(self.strings) + 1))
		self.strings[string] = value


	def largestLayerSize(self):
		return QSize(max(self.layers, key=lambda l: l.width).width, max(self.layers, key=lambda l: l.height).height)


	def getTileIndexAt(self, tileX:int, tileY:int, layer:int):
		if layer < 0 or layer >= len(self.layers):
			print(f"Invalid layer: {layer}")
			return -1

		if tileX < 0 or tileX >= self.layers[layer].width or tileY < 0 or tileY >= self.layers[layer].height:
			print(f"Invalid position: {tileX},{tileY}")
			return -1
		tileIndex = tileX + self.layers[layer].width * tileY
		return self.layers[layer].tiles[tileIndex]


	def setTileIndexAt(self, tileX:int, tileY:int, layer:int, newIndex:int):
		if layer < 0 or layer >= len(self.layers):
			raise IndexError(f"Invalid layer index {layer}")

		if tileX < 0 or tileX >= self.layers[layer].width or tileY < 0 or tileY >= self.layers[layer].height:
			raise IndexError(f"Invalid position {tileX},{tileY}")

		tileIndex = tileX + self.layers[layer].width * tileY
		self.layers[layer].tiles[tileIndex] = newIndex


	def entitiesOnTile(self, tileX:int, tileY:int, layer:int) -> list[MapEntity]:
		if layer < 0 or layer >= len(self.layers):
			raise IndexError(f"Invalid layer index {layer}")

		tw = self.tileset.tileWidth
		th = self.tileset.tileHeight
		mx = tileX * tw
		my = tileY * th
		entities = []
		for entity in self.entities:
			if entity.layer != layer:
				continue

			if mx <= entity.mapX and entity.mapX <= mx + tw and \
				my <= entity.mapY and entity.mapY <= my + th:
				entities.append(entity)

		return entities
