from os import path
from PySide6.QtGui import QUndoCommand

from commands.commandids import CommandIDs
from dialogs.mappropertiesdialog import MapPropertiesDialog
from formats.spheremap import MapString, SphereMap
from formats.tileset import Tileset

# this may end up being removed and properties edited directly (making property editing undoable)
class MapPropertiesChangedCommand(QUndoCommand):
	sphereMap: SphereMap

	oldTileset: str
	newTileset: str
	oldBGM: str
	newBGM: str
	oldRepeating: bool
	newRepeating: bool
	oldEntryScript: str
	newEntryScript: str
	oldExitScript: str
	newExitScript: str
	oldNorthScript: str
	newNorthScript: str
	oldEastScript: str
	newEastScript: str
	oldSouthScript: str
	newSouthScript: str
	oldWestScript: str
	newWestScript: str
	def __init__(self, dialog: MapPropertiesDialog, sphereMap: SphereMap, editor):
		super().__init__()
		self.sphereMap = sphereMap
		self.editor = editor
		self.oldTileset = sphereMap.tilesetFile
		self.newTileset = dialog.tileset
		self.oldBGM = sphereMap.getString(MapString.MusicFile)
		self.newBGM = dialog.bgm
		self.oldRepeating = sphereMap.repeating
		self.newRepeating = dialog.repeating
		self.oldEntryScript = sphereMap.getString(MapString.EntryScript)
		self.newEntryScript = dialog.entryScript
		self.oldExitScript = sphereMap.getString(MapString.ExitScript)
		self.newExitScript = dialog.exitScript
		self.oldNorthScript = sphereMap.getString(MapString.NorthScript)
		self.newNorthScript = dialog.northScript
		self.oldEastScript = sphereMap.getString(MapString.EastScript)
		self.newEastScript = dialog.eastScript
		self.oldSouthScript = sphereMap.getString(MapString.SouthScript)
		self.newSouthScript = dialog.southScript
		self.oldWestScript = sphereMap.getString(MapString.WestScript)
		self.newWestScript = dialog.westScript


	
	def id(self) -> int:
		return CommandIDs.MapPropertiesChanged.value


	def undo(self):
		self.sphereMap.tilesetFile = self.oldTileset
		self.sphereMap.setString(MapString.MusicFile, self.oldBGM)
		self.sphereMap.repeating = self.oldRepeating
		self.sphereMap.setString(MapString.EntryScript, self.oldEntryScript)
		self.sphereMap.setString(MapString.ExitScript, self.oldExitScript)
		self.sphereMap.setString(MapString.NorthScript, self.oldNorthScript)
		self.sphereMap.setString(MapString.EastScript, self.oldEastScript)
		self.sphereMap.setString(MapString.SouthScript, self.oldSouthScript)
		self.sphereMap.setString(MapString.WestScript, self.oldWestScript)
		self.__reloadTileset(self.oldTileset)


	def redo(self):
		self.sphereMap.tilesetFile = self.newTileset
		self.sphereMap.setString(MapString.MusicFile, self.newBGM)
		self.sphereMap.repeating = self.newRepeating
		self.sphereMap.setString(MapString.EntryScript, self.newEntryScript)
		self.sphereMap.setString(MapString.ExitScript, self.newExitScript)
		self.sphereMap.setString(MapString.NorthScript, self.newNorthScript)
		self.sphereMap.setString(MapString.EastScript, self.newEastScript)
		self.sphereMap.setString(MapString.SouthScript, self.newSouthScript)
		self.sphereMap.setString(MapString.WestScript, self.newWestScript)
		self.__reloadTileset(self.newTileset)


	def __reloadTileset(self, tilesetPath: str):
		if not path.isabs(tilesetPath):
			tilesetPath = path.join(path.dirname(self.sphereMap.filePath), tilesetPath)
		rts = Tileset(tilesetPath)
		rts.open()
		self.editor.attachTileset(rts)


	def mergeWith(self, other: QUndoCommand) -> bool:
		return isinstance(other, MapPropertiesChangedCommand) and other.id() == self.id() and \
			other.sphereMap.filePath == self.sphereMap.filePath and \
			other.newTileset == self.newTileset and \
			other.newBGM == self.newBGM and \
			other.newRepeating == self.newRepeating and \
			other.newEntryScript == self.newEntryScript and \
			other.newExitScript == self.newExitScript and \
			other.newNorthScript == self.newNorthScript and \
			other.newEastScript == self.newEastScript and \
			other.newSouthScript == self.newSouthScript and \
			other.newWestScript == self.newWestScript

