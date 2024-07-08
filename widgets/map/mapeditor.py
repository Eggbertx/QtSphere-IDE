from PySide6.QtWidgets import QWidget

from formats.spheremap import SphereMap
from widgets.sphereeditor import SphereEditor

from ui.ui_mapeditor import Ui_MapEditor

class MapEditor(SphereEditor):
	ui: Ui_MapEditor
	map: SphereMap
	def __init__(self, parent: QWidget | None = None):
		super().__init__(parent)
		self.ui = Ui_MapEditor()
		self.ui.setupUi(self)
		self.map = None
	
	def attachMap(self, map:SphereMap):
		self.map = map
