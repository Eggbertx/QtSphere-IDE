from PySide6.QtCore import Qt
from PySide6.QtWidgets import QDialog, QWidget

from ui.ui_entityeditdialog import Ui_EntityEditDialog

from formats.spheremap import MapPerson, MapTrigger, SphereMap

class EntityEditDialog(QDialog):
	ui: Ui_EntityEditDialog
	scripts:list[str]

	def __init__(self, parent: QWidget):
		super().__init__(parent)
		self.ui = Ui_EntityEditDialog()
		self.ui.setupUi(self)
		self.scripts = ["","","","","","","",""]
		self.ui.scriptAction_cmb.currentIndexChanged.connect(lambda i: self.updateCurrentScriptIndex(i))

	def show(self, entity: MapPerson|MapTrigger, map:SphereMap):
		super().show()

		if hasattr(entity, "spritesetFilename"):
			# person entity
			self.ui.name_txt.setText(entity.name)
			self.ui.spriteset_txt.setText(entity.spritesetFilename)
			self.scripts = entity.scripts
			self.updateCurrentScriptIndex(self.ui.layersCombo.currentIndex())
		else:
			# trigger entity
			self.updateCurrentScriptIndex(0)

		for layer in map.layers:
			self.ui.layersCombo.addItem(layer.name)
		self.ui.layersCombo.setCurrentIndex(entity.layer)

	def updateCurrentScriptIndex(self, i:int):
		self.ui.scriptEdit.setPlainText(self.scripts[i])