from os import path
from PySide6.QtCore import Slot
from PySide6.QtWidgets import QDialog, QFileDialog, QMessageBox
from formats.spheremap import MapString, SphereMap
from formats.tileset import Tileset

from ui.ui_mappropertiesdialog import Ui_MapPropertiesDialog

class MapPropertiesDialog(QDialog):
	map:SphereMap
	ui:Ui_MapPropertiesDialog
	def __init__(self, parent = None, map:SphereMap = None):
		super().__init__(parent)
		self.ui = Ui_MapPropertiesDialog()
		self.ui.setupUi(self)
		self.ui.buttonBox.accepted.connect(self.accept)
		self.ui.buttonBox.rejected.connect(self.reject)
		self.ui.tileset_btn.clicked.connect(self.onOpenTilesetClicked)
		self.map = map
		self.updateUI()

	def updateUI(self):
		self.setWindowTitle(f"Map Properties - {path.basename(self.map.filePath)}")
		self.ui.tileset_txt.setText(self.map.tileset.filePath)
		self.ui.bgm_txt.setText(self.map.getString(MapString.MusicFile) or "")
		self.ui.bgm_txt.setEnabled(self.map.getString(MapString.MusicFile) != "") # deprecated and rarely used

		self.ui.layers_lbl.setText(f"{len(self.map.layers)} layers")
		self.ui.entities_lbl.setText(f"{len(self.map.entities)} entities")
		self.ui.zones_lbl.setText(f"{len(self.map.zones)} zones")
		self.ui.repeat_chk.setChecked(self.map.repeating)

		self.ui.enter_txt.setPlainText(self.map.getString(MapString.EntryScript) or "")
		self.ui.leave_txt.setPlainText(self.map.getString(MapString.ExitScript) or "")
		self.ui.north_txt.setPlainText(self.map.getString(MapString.NorthScript) or "")
		self.ui.east_txt.setPlainText(self.map.getString(MapString.EastScript) or "")
		self.ui.south_txt.setPlainText(self.map.getString(MapString.SouthScript) or "")
		self.ui.west_txt.setPlainText(self.map.getString(MapString.WestScript) or "")

	@Slot()
	def onOpenTilesetClicked(self):
		mapDir = path.dirname(self.map.filePath)
		fileName, _ = QFileDialog.getOpenFileName(self, "Open Tileset", mapDir, "Tileset (*.rts);;All files (*.*)")
		if fileName:
			if fileName == self.map.tileset.filePath:
				return
			numTiles = len(self.map.tileset.tiles)
			rts = Tileset(fileName)
			rts.open()
			if len(rts.tiles) != numTiles:
				result = QMessageBox.warning(self, "Tileset Size Mismatch",
					"The new tileset has a different number of tiles than the existing one. This may cause issues with existing map. Do you want to continue?",
					QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No)
				if result == QMessageBox.StandardButton.Yes:
					self.ui.tileset_txt.setText(path.relpath(fileName, mapDir))


	def exec(self):
		self.ui.tabWidget.setCurrentIndex(0)
		return super().exec()