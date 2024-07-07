from io import BufferedReader
import struct

from formats.spherefile import SphereFile, FormatException, readSphereString

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
	TilesetFile = 0 # obsolete(?)
	MusicFile = 1
	ScriptFile = 2 # obsolete(?)
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
		return (~self.flags) & 1
	
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
	tiles:tuple[int]
	segments:list[ObstructionSegment]

	@staticmethod
	def fromReader(reader:BufferedReader):
		layer = MapLayer()
		(layer.width, layer.height, layer.flags, layer.parallaxX, layer.parallaxY, layer.scrollingX,
		layer.scrollingY, layer.numSegments, layer.reflective) = struct.unpack("<3H4fL?3x", reader.read(30))
		layer.name = readSphereString(reader)
		layer.tiles = struct.unpack(f"<{layer.width*layer.height}H", reader.read(layer.width*layer.height*2))
		layer.segments = []
		for s in range(layer.numSegments):
			segment = ObstructionSegment()
			(segment.x1, segment.y1, segment.x2, segment.y2) = struct.unpack("<4L", reader.read(16))
			layer.segments.append(segment)
		return layer


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

	@staticmethod
	def fromReader(reader: BufferedReader, filePath:str):
		zone = MapZone()
		(zone.x1,zone.y1,zone.x2,zone.y2,zone.layer,zone.reactivateInNumSteps) = struct.unpack("<6H4x", reader.read(16))
		return zone


class SphereMap(SphereFile):
	startX: int
	startY: int
	startLayer: int
	startDirection: MapDirections

	strings:list[str]
	layers:list[MapLayer]
	entities:list[MapEntity]

	def __init__(self, filePath: str = None):
		super().__init__(filePath)
		self.strings = []
		self.layers = []
		self.entities = []
		self.zones = []

	def _parseFileData(self, file: BufferedReader):
		super()._parseFileData(file)
		file.seek(0)
		if file.read(4) != b".rmp":
			raise FormatException(self.filePath, f"invalid file signature")
		
		(version, numLayers, numEntities, self.startX, self.startY,
		self.startLayer, self.startDirection, numStrings, numZones) = struct.unpack("<HxBx3H2B2H235x", file.read(252))
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
			zone = MapZone.fromReader(file, self.filePath)
			if zone.layer < 0 or zone.layer >= numLayers:
				raise FormatException(self.filePath, f"Invalid zone layer {zone.layer}, expected layer between 0 and {numLayers}")
			self.zones.append(zone)

	def _packBytes() -> bytes:
		raise NotImplementedError()