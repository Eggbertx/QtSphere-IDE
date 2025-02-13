from enum import Enum,auto

class DrawingTool(Enum):
	Pencil = auto()
	Line = auto()
	Rectangle = auto()
	RectangleFilled = auto()
	Fill = auto()
	Dropper = auto()
	Select = auto()

class DrawMode(Enum):
	NotDrawing = auto()
	MouseDown = auto()
	MouseReleased = auto()
