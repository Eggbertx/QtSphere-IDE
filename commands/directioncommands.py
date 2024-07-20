from PySide6.QtGui import QUndoCommand
from PySide6.QtWidgets import QWidget

from formats.spriteset import SpritesetFrame
from commands.commandids import CommandIDs

class AddRemoveDirectionFrameCommand(QUndoCommand):
	view: QWidget
	directionIndex:int
	initialFrames:int
	affectedFrame:SpritesetFrame
	isRemove:bool
	
	@property
	def spriteset(self):
		return self.view.spriteset

	@property
	def direction(self):
		return self.spriteset.directions[self.directionIndex]


	def __init__(self, view:QWidget, direction:int, isRemove:bool):
		super().__init__(None)
		self.view = view
		self.directionIndex = direction
		self.initialFrames = len(view.spriteset.directions[direction].frames)
		self.affectedFrame = None
		self.isRemove = isRemove


	def __addFrame(self):
		self.direction.frames.append(self.affectedFrame)
		self.view.addFrame(self.spriteset.images[0])


	def __removeFrame(self):
		self.affectedFrame = self.direction.frames.pop()
		self.view.removeFrame()


	def id(self) -> int:
		return CommandIDs.DirectionAdd


	def undo(self):
		if self.isRemove:
			self.__addFrame()
		else:
			self.__removeFrame()


	def redo(self):
		if self.isRemove:
			self.__removeFrame()
		else:
			self.__addFrame()


	def mergeWith(self, other: QUndoCommand) -> bool:
		if other.id() == CommandIDs.DirectionAdd and hasattr(other, "addedFrame") and other.initialFrames == self.initialFrames and other.isRemove == self.isRemove:
			self.setText("Remove frame" if self.isRemove else "Add frame")
			return True
		return False
