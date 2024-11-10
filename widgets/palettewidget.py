from enum import Enum, auto
from os.path import splitext

from PySide6.QtCore import Qt, QPoint, QRect, Slot, QDir
from PySide6.QtGui import QColor, QContextMenuEvent, QMouseEvent, QPaintEvent, QPainter
from PySide6.QtWidgets import QWidget, QMenu, QColorDialog, QFileDialog

from palette_presets import c64_palette, nes_palette, dos_palette, hsl256_palette, plasma_palette, rgb332_palette,verge_palette, visibone2_palette

class ChangeColorOption(Enum):
	InsertBefore = auto()
	InsertAfter = auto()
	Replace = auto()
	Remove = auto()

class PaletteWidget(QWidget):
	rightClickMenu: QMenu
	fileMenu: QMenu
	defaultMenu: QMenu
	mousePos: QPoint
	selectedIndex: int
	squareSize: int
	paletteColors: list[QColor]

	def __init__(self, parent: QWidget | None = None):
		super().__init__(parent)
		self.selectedIndex = 0
		self.squareSize = 10
		
		# self.parentWidget().setMinimumSize(self.squareSize * 16, self.squareSize * 16)
		self.changePalette(hsl256_palette)

		self.rightClickMenu = QMenu(self)
		self.fileMenu = self.rightClickMenu.addMenu("File")
		self.fileMenu.addAction("New", self.onNewPaletteSelected)
		self.fileMenu.addAction("Import...", self.onImportPaletteSelected)
		self.fileMenu.addAction("Export...", self.onExportPaletteSelected)

		self.defaultMenu = self.rightClickMenu.addMenu("Presets")
		self.defaultMenu.addAction("Commodore 64", lambda: self.changePalette(c64_palette))
		self.defaultMenu.addAction("DOS", lambda: self.changePalette(dos_palette))
		self.defaultMenu.addAction("HSL256", lambda: self.changePalette(hsl256_palette))
		self.defaultMenu.addAction("NES", lambda: self.changePalette(nes_palette))
		self.defaultMenu.addAction("PLASMA", lambda: self.changePalette(plasma_palette))
		self.defaultMenu.addAction("RGB332", lambda: self.changePalette(rgb332_palette))
		self.defaultMenu.addAction("VERGE", lambda: self.changePalette(verge_palette))
		self.defaultMenu.addAction("VISIBONE2", lambda: self.changePalette(visibone2_palette))

		self.rightClickMenu.addSeparator()
		self.rightClickMenu.addAction("Insert before")
		self.rightClickMenu.addAction("Insert after")
		self.rightClickMenu.addAction("Replace")
		self.rightClickMenu.addAction("Remove")


	def changePalette(self, palette: list[QColor]):
		self.paletteColors = list(palette)
		self.repaint()


	def importPalette(self, path:str):
		with open(path, "r") as fi:
			self.paletteColors.clear()
			lines = fi.readlines()
			for line in lines:
				color = QColor(line)
				if color.isValid():
					self.paletteColors.append(color)
			self.repaint()


	def exportPalette(self, path:str):
		if splitext(path)[1] == "":
			path += ".qsipal"
		with open(path, "w") as fi:
			for c in range(len(self.paletteColors)):
				fi.write(self.paletteColors[c].name() + "\n")

#region Event overloads

	def paintEvent(self, event: QPaintEvent):
		painter = QPainter(self)
		x = 0
		y = 0
		for c in range(len(self.paletteColors)):
			if x + self.squareSize > self.width():
				x = 0
				y += self.squareSize

			if self.selectedIndex == c:
				painter.fillRect(QRect(x, y, self.squareSize, self.squareSize), Qt.GlobalColor.black)
				painter.fillRect(QRect(x + 1, y + 1, self.squareSize - 2, self.squareSize - 2), Qt.GlobalColor.white)
				painter.fillRect(QRect(x + 2, y + 2, self.squareSize - 4, self.squareSize - 4), Qt.GlobalColor.black)
				painter.fillRect(QRect(x + 3, y + 3, self.squareSize - 6, self.squareSize - 6), self.paletteColors[c])
			else:
				painter.fillRect(QRect(x, y, self.squareSize, self.squareSize), self.paletteColors[c])

			x += self.squareSize


	def mousePressEvent(self, event: QMouseEvent):
		buttonPressed = event.button()
		pos = event.pos()
		width = self.width()
		if buttonPressed == Qt.MouseButton.LeftButton or buttonPressed == Qt.MouseButton.RightButton:
			x = 0
			y = 0
			for c in range(len(self.paletteColors)):
				if x + self.squareSize > width:
					x = 0
					y += self.squareSize
				
				if x <= pos.x() and pos.x() <= x + self.squareSize and y <= pos.y() and pos.y() <= y + self.squareSize:
					self.selectedIndex = c
					self.repaint()
				x += self.squareSize
		if buttonPressed == Qt.MouseButton.RightButton:
			self.rightClickMenu.exec(event.globalPos())

#endregion

#region Slots
	@Slot()
	def onNewPaletteSelected(self):
		self.selectedIndex = 0
		self.paletteColors.clear()
		self.paletteColors.append(QColor())
		self.repaint()


	@Slot()
	def onImportPaletteSelected(self):
		importPath = QFileDialog.getOpenFileName(self, "Import Palette", QDir().absolutePath(),
			"QtSphere IDE palettes (*.qsipal);;All files (*.*)")

		if importPath != "" and importPath is not None:
			self.importPalette(importPath);


	@Slot()
	def onExportPaletteSelected(self):
		exportPath = QFileDialog.getSaveFileName(self, "Export Palette", QDir().absolutePath(),
			"QtSphere IDE palettes (*.qsipal);;All files (*.*)")

		if exportPath != "":
			self.exportPalette(exportPath)


	@Slot(ChangeColorOption)
	def onChangeColorSelected(self, option: ChangeColorOption):
		if option == ChangeColorOption.Remove:
			self.paletteColors.remove(self.selectedIndex)
			self.repaint()
			return

		newCol = QColorDialog.getColor(Qt.white, self)
		if not newCol.isValid():
			return

		match option:
			case ChangeColorOption.InsertBefore:
				self.paletteColors.insert(self.selectedIndex, newCol)
				self.selectedIndex += 1
			case ChangeColorOption.InsertAfter:
				self.paletteColors.insert(self.selectedIndex + 1, newCol)
			case ChangeColorOption.Replace:
				self.paletteColors[self.selectedIndex] = newCol
		self.repaint()
#endregion
