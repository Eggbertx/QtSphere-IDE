from PySide6.QtGui import QColor, QUndoCommand

from commands.commandids import CommandIDs
from dialogs.windowstylepropertiesdialog import WindowStylePropertiesDialog
from formats.windowstyle import SphereWindowStyle
# from widgets.windowstyle.windowstyleeditor import WindowStyleEditor

class WindowStylePropertiesChangedCommand(QUndoCommand):
	dialog: WindowStylePropertiesDialog
	windowStyle: SphereWindowStyle
	oldBackgroundMode: int
	newBackgroundMode: int
	oldCornerColors: list[QColor]
	newCornerColors: list[QColor]
	oldEdgeOffsets: list[int]
	newEdgeOffsets: list[int]

	def __init__(self, dialog: WindowStylePropertiesDialog, windowStyle: SphereWindowStyle, editor):
		super().__init__()
		self.dialog = dialog
		self.windowStyle = windowStyle
		self.editor = editor
		self.oldBackgroundMode = windowStyle.backgroundMode
		self.newBackgroundMode = dialog.backgroundMode
		self.oldCornerColors = windowStyle.cornerColors.copy()
		self.newCornerColors = [
			dialog.ui.ulColor_btn.color,
			dialog.ui.urColor_btn.color,
			dialog.ui.llColor_btn.color,
			dialog.ui.lrColor_btn.color
		]
		self.oldEdgeOffsets = windowStyle.edgeOffsets.copy()
		self.newEdgeOffsets = [
			dialog.ui.leftOffset_num.value(),
			dialog.ui.topOffset_num.value(),
			dialog.ui.rightOffset_num.value(),
			dialog.ui.bottomOffset_num.value()
		]
	
	def id(self) -> int:
		return CommandIDs.WindowStylePropertiesChanged.value


	def undo(self):
		self.windowStyle.backgroundMode = self.oldBackgroundMode
		for i in range(4):
			self.windowStyle.cornerColors[i] = self.oldCornerColors[i]
			self.windowStyle.edgeOffsets[i] = self.oldEdgeOffsets[i]
		self.editor.attachWindowStyle(self.windowStyle)


	def redo(self):
		self.windowStyle.backgroundMode = self.newBackgroundMode
		for i in range(4):
			self.windowStyle.cornerColors[i] = self.newCornerColors[i]
			self.windowStyle.edgeOffsets[i] = self.newEdgeOffsets[i]
		self.editor.attachWindowStyle(self.windowStyle)


	def mergeWith(self, other: QUndoCommand) -> bool:
		return isinstance(other, WindowStylePropertiesChangedCommand) and \
			other.id() == self.id() and \
			other.windowStyle.filePath == self.windowStyle.filePath and \
			other.newBackgroundMode == self.newBackgroundMode and \
			other.newCornerColors == self.newCornerColors and \
			other.newEdgeOffsets == self.newEdgeOffsets