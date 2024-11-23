from enum import Enum, auto

class CommandIDs(Enum):
	DirectionAdd = auto()
	DirectionRemove = auto()
	InsertTiles = auto()
	AppendTiles = auto()
	RemoveTiles = auto()