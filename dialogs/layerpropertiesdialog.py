from PySide6.QtCore import Slot
from PySide6.QtWidgets import QDialog, QWidget

from ui.ui_layerpropertiesdialog import Ui_LayerPropertiesDialog

from formats.spheremap import MapLayer, SphereMap

class LayerPropertiesDialog(QDialog):
	ui: Ui_LayerPropertiesDialog


	@property
	def parallaxX(self):
		return self.ui.horizontalParallaxSlider.value() / 10

	@property
	def parallaxY(self):
		return self.ui.verticalParallaxSlider.value() / 10

	@property
	def scrollX(self):
		return self.ui.horizontalAsSlider.value() / 10

	@property
	def scrollY(self):
		return self.ui.verticalAsSlider.value() / 10


	def __init__(self, parent: QWidget = None):
		super().__init__(parent)
		self.ui = Ui_LayerPropertiesDialog()
		self.ui.setupUi(self)
		self.ui.horizontalParallaxSlider.valueChanged.connect(lambda v: self.ui.horizontalParallaxIndicator.setText(f"{v/10}"))
		self.ui.verticalParallaxSlider.valueChanged.connect(lambda v: self.ui.verticalParallaxIndicator.setText(f"{v/10}"))
		self.ui.horizontalAsSlider.valueChanged.connect(lambda v: self.ui.horizontalAsIndicator.setText(f"{v/10}"))
		self.ui.verticalAsSlider.valueChanged.connect(lambda v: self.ui.verticalAsIndicator.setText(f"{v/10}"))


	def __setParallaxScrollControlVals(self, layer:MapLayer):
		self.ui.parallaxChk.setEnabled(True)
		self.ui.parallaxChk.setChecked(layer.hasParallax)
		self.ui.horizontalParallaxSlider.setEnabled(True)
		self.ui.horizontalParallaxSlider.setValue(layer.parallaxX * 10)
		self.ui.horizontalParallaxIndicator.setText(f"{layer.parallaxX}")
		self.ui.verticalParallaxSlider.setEnabled(True)
		self.ui.verticalParallaxSlider.setValue(layer.parallaxY * 10)
		self.ui.verticalParallaxIndicator.setText(f"{layer.parallaxY}")

		self.ui.horizontalAsSlider.setEnabled(True)
		self.ui.horizontalAsSlider.setValue(layer.scrollingX * 10)
		self.ui.horizontalAsIndicator.setText(f"{layer.scrollingX}")
		self.ui.verticalAsSlider.setEnabled(True)
		self.ui.verticalAsSlider.setValue(layer.scrollingY * 10)
		self.ui.verticalAsIndicator.setText(f"{layer.scrollingY}")


	def show(self, layerIndex:int, map:SphereMap):
		layer = map.layers[layerIndex]
		self.ui.nameLineEdit.setText(layer.name)
		self.ui.widthSpinBox.setValue(layer.width)
		self.ui.heightSpinBox.setValue(layer.height)
		self.ui.reflective_chk.setChecked(layer.reflective)

		if layer.hasParallax or layer.scrollingX + layer.scrollingY > 0:
			self.__setParallaxScrollControlVals(layer)
		else:
			for mLayer in map.layers:
				if not mLayer.hasParallax:
					self.__setParallaxScrollControlVals(layer)
		return super().show()
