from PySide6.QtGui import QColor, QImage
from PySide6.QtCore import Qt


from .spherefile import FormatException, SphereFile

class BackgroundMode:
	Tiled = 0
	Stretched = 1
	Gradient = 2
	TiledWithGradient = 3
	StretchedWithGradient = 4

class GradientCorner:
	UpperLeft = 0
	UpperRight = 1
	LowerLeft = 2
	LowerRight = 3

class EdgeOffset:
	Left = 0
	Top = 1
	Right = 2
	Bottom = 3

class WindowstyleBitmap:
	UpperLeft = 0
	Top = 1
	UpperRight = 2
	Right = 3
	LowerRight = 4
	Bottom = 5
	LowerLeft = 6
	Left = 7
	Background = 8
	

class SphereWindowStyle(SphereFile):
	version: int
	edgeWidth: int
	backgroundMode: BackgroundMode

	__cornerColors: list[QColor]
	@property
	def cornerColors(self) -> list[QColor]:
		return self.__cornerColors
	@cornerColors.setter
	def cornerColors(self, colors: list[QColor]):
		if len(colors) != 4:
			raise ValueError("cornerColors must be a list of 4 QColors")
		self.__cornerColors = colors

	__edgeOffsets: list[int]
	@property
	def edgeOffsets(self) -> list[int]:
		return self.__edgeOffsets
	@edgeOffsets.setter
	def edgeOffsets(self, offsets: list[int]):
		if len(offsets) != 4:
			raise ValueError("edgeOffsets must be a list of 4 integers")
		self.__edgeOffsets = offsets

	__bitmaps: list[QImage]
	@property
	def bitmaps(self) -> list[QImage]:
		return self.__bitmaps
	@bitmaps.setter
	def bitmaps(self, images: list[QImage]):
		if len(images) != 9:
			raise ValueError("bitmaps must be a list of 9 QImages")
		self.__bitmaps = images

	def __init__(self, filePath = None):
		super().__init__(filePath)
		self.version = 0
		self.edgeWidth = 0
		self.backgroundMode = BackgroundMode.Tiled
		self.cornerColors = [
			Qt.GlobalColor.black,
			Qt.GlobalColor.black,
			Qt.GlobalColor.black,
			Qt.GlobalColor.black
		]
		self.edgeOffsets = [0, 0, 0, 0]
		self.bitmaps = [QImage() for _ in range(9)]

	def _parseFileData(self, file):
		super()._parseFileData(file)
		file.seek(0)
		if file.read(4) != b".rws":
			raise FormatException(self.filePath, "Invalid file signature")
		self.version = int.from_bytes(file.read(2), "little")
		if self.version not in (1, 2):
			raise FormatException(self.filePath, f"Unsupported windowstyle version {self.version}")
		
		self.edgeWidth = file.read(1)[0]
		if self.version == 2:
			self.edgeWidth = 0 # Version 2 doesn't use edge width

		self.backgroundMode = file.read(1)[0]
		self.cornerColors = [QColor(file.read(1)[0], file.read(1)[0], file.read(1)[0], file.read(1)[0]) for _ in range(4)]
		self.edgeOffsets = [file.read(1)[0] for _ in range(4)]
		file.read(36) # Reserved bytes
		match self.version:
			case 1:
				self.bitmaps = [
					QImage(file.read(self.edgeWidth * self.edgeWidth * 4), self.edgeWidth, self.edgeWidth, QImage.Format.Format_RGBA8888) for _ in range(9)
				]
			case 2:
				for i in range(9):
					width = int.from_bytes(file.read(2), "little")
					height = int.from_bytes(file.read(2), "little")
					img_data = file.read(width * height * 4)
					self.bitmaps[i] = QImage(img_data, width, height, QImage.Format.Format_RGBA8888)


	def _packBytes(self):
		raise NotImplementedError()