from PySide6.QtGui import QUndoCommand
from PySide6.QtWidgets import QWidget

from formats.spriteset import SpritesetFrame, SphereSpriteset
from commands.commandids import CommandIDs

class DirectionAppendFrameCommand(QUndoCommand):
	view: QWidget
	dirIndex:int
	initialFrameCount:int

	@property
	def spriteset(self):
		return self.view.spriteset

	@property
	def direction(self):
		return self.spriteset.directions[self.dirIndex]

	def __init__(self, view:QWidget, direction:int):
		super().__init__(None)
		self.view = view
		self.dirIndex = direction
		self.initialFrames = len(view.spriteset.directions[direction].frames)


	def redo(self):
		self.direction.frames.append(SpritesetFrame(0,8))
		self.view.addFrame(self.spriteset.images[0])
	

	def undo(self):
		self.affectedFrame = self.direction.frames.pop()
		self.view.removeFrame()


	def mergeWith(self, other: QUndoCommand) -> bool:
		if other.id() == CommandIDs.DirectionAdd and isinstance(other, DirectionAppendFrameCommand) and other.initialFrames == self.initialFrames and other.dirIndex == self.dirIndex:
			self.setText("Add frame")
			return True
		return False



class DirectionRemoveLastFrameCommand(QUndoCommand):
	view: QWidget
	dirIndex:int
	initialFrames:int
	affectedFrame:SpritesetFrame

	@property
	def spriteset(self) -> SphereSpriteset:
		return self.view.spriteset

	@property
	def direction(self):
		return self.spriteset.directions[self.dirIndex]

	def __init__(self, view:QWidget, direction:int):
		super().__init__(None)
		self.view = view
		self.dirIndex = direction
		self.initialFrames = len(view.spriteset.directions[direction].frames)
		noFrames = len(self.spriteset.directions[direction].frames)
		self.affectedFrame = None


	def undo(self):
		self.direction.frames.append(self.affectedFrame)
		self.view.addFrame(self.spriteset.images[self.affectedFrame.imageIndex])


	def redo(self):
		self.affectedFrame = self.direction.frames.pop()
		self.view.removeFrame()


	def mergeWith(self, other: QUndoCommand) -> bool:
		if other.id() == CommandIDs.DirectionRemove and isinstance(other, DirectionRemoveLastFrameCommand) and other.initialFrames == self.initialFrames and other.affectedFrame == self.affectedFrame:
			self.setText("Remove frame")
			return True
		return False