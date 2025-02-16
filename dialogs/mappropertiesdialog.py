from os.path import basename
from PySide6.QtWidgets import QDialog
from formats.spheremap import SphereMap

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
		self.map = map
		self.setWindowTitle(f"Map Properties - {basename(map.filePath)}")
		self.ui.tileset_txt.setText(map.tileset.filePath)
		self.ui.bgm_txt.setText(map.musicFile)
		self.ui.bgm_txt.setEnabled(map.musicFile != "") # deprecated and rarely used

		self.ui.enter_txt.setPlainText(map.strings[3])
		self.ui.leave_txt.setPlainText(map.strings[4])
		self.ui.north_txt.setPlainText(map.strings[5])
		self.ui.east_txt.setPlainText(map.strings[6])
		self.ui.south_txt.setPlainText(map.strings[7])
		self.ui.west_txt.setPlainText(map.strings[8])

		self.ui.layers_lbl.setText(f"{len(map.layers)} layers")
		self.ui.entities_lbl.setText(f"{len(map.entities)} entities")
		self.ui.zones_lbl.setText(f"{len(map.zones)} zones")
		self.ui.repeat_chk.setChecked(map.repeating)

