from PySide6.QtCore import Qt, QPoint, Slot
from PySide6.QtGui import QImage, QIcon, QBrush, QPixmap
from PySide6.QtWidgets import QWidget, QFrame, QHBoxLayout, QLineEdit, QGraphicsView, QGraphicsScene, QToolButton, QMenu, QGraphicsPixmapItem

from formats.spriteset import SphereSpriteset

class DirectionView(QFrame):
	spriteset: SphereSpriteset
	directionIndex: int
	zoomFactor: float
	nameEdit: QLineEdit
	framesContainer: QWidget
	framesLayout: QHBoxLayout
	frameViews: list[QGraphicsView]
	addFrameButton: QToolButton
	removeFrameButton: QToolButton
	contextMenu: QMenu

	@property
	def direction(self):
		return self.spriteset.directions[self.directionIndex]

	def __init__(self, parent: QWidget, spriteset: SphereSpriteset, index:int) -> None:
		super().__init__(parent)
		self.spriteset = spriteset
		self.directionIndex = index
		self.zoomFactor = 2

		self.setContextMenuPolicy(Qt.ContextMenuPolicy.CustomContextMenu)
		self.customContextMenuRequested.connect(self.showContextMenu)

		dirLayout = QHBoxLayout()
		dirLayout.setSpacing(0)
		dirLayout.setContentsMargins(0, 0, 0, 0)
		self.setLayout(dirLayout)

		self.nameEdit = QLineEdit(self.direction.name)
		self.nameEdit.setFixedWidth(100)
		self.nameEdit.textChanged.connect(self.nameChanged)
		dirLayout.addWidget(self.nameEdit)

		self.framesContainer = QWidget()
		self.framesLayout = QHBoxLayout()
		self.framesLayout.setSpacing(1)
		self.framesLayout.setContentsMargins(2, 2, 2, 2)
		self.framesContainer.setLayout(self.framesLayout)

		self.frameViews = []
		for frame in self.direction.frames:
			self.addFrame(self.spriteset.images[frame.imageIndex])
		dirLayout.addWidget(self.framesContainer)

		self.removeFrameButton = QToolButton(self)
		self.removeFrameButton.setText("-")
		self.removeFrameButton.setToolTip("Remove a frame")
		self.removeFrameButton.setIcon(QIcon.fromTheme("list-remove"))
		self.removeFrameButton.clicked.connect(self.frameRemoveClicked)
		dirLayout.addWidget(self.removeFrameButton)

		self.addFrameButton = QToolButton(self)
		self.addFrameButton.setText("+")
		self.addFrameButton.setToolTip("Add a frame")
		self.addFrameButton.setIcon(QIcon.fromTheme("list-add"))
		self.addFrameButton.clicked.connect(self.frameAddClicked)
		dirLayout.addWidget(self.addFrameButton)
		dirLayout.addStretch(5)

		self.contextMenu = QMenu(self)
		self.contextMenu.addAction("Insert")
		self.contextMenu.addAction("Append")
		self.contextMenu.addAction("Delete")
		self.contextMenu.addSeparator()
		self.contextMenu.addAction("Insert frames...")
		self.contextMenu.addAction("Append frames...")
		self.contextMenu.addAction("Delete frames...")
		self.contextMenu.addSeparator()
		self.contextMenu.addAction("Cut (Ctrl+X)")
		self.contextMenu.addAction("Copy (Ctrl+C)")
		self.contextMenu.addAction("Paste (Ctrl+V)")
		self.contextMenu.addSeparator()
		self.contextMenu.addAction("Insert from image")
		self.contextMenu.addAction("Append from image")

	def addFrame(self, img: QImage):
		scene = QGraphicsScene()
		view = QGraphicsView(scene, self)
		view.setScene(scene)
		view.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
		view.setVerticalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
		view.setBackgroundBrush(QBrush(QPixmap(":/icons/transparency-bg.png")))

		pixmap = QPixmap.fromImage(img if img is not None else self.spriteset.images[0])
		item = QGraphicsPixmapItem(pixmap)
		scaledWidth = pixmap.width() * self.zoomFactor
		scaledHeight = pixmap.height() * self.zoomFactor

		scene.addItem(item)
		item.setScale(self.zoomFactor)
		view.setFixedSize(scaledWidth, scaledHeight)

		self.framesLayout.addWidget(view)
		self.frameViews.append(view)

	def removeFrame(self):
		if len(self.frameViews) < 2:
			return
		toRemove = self.frameViews.pop(len(self.frameViews)-1)
		self.framesLayout.removeWidget(toRemove)

	@Slot()
	def frameAddClicked(self):
		self.addFrame(self.spriteset.images[0])

	@Slot()
	def frameRemoveClicked(self):
		self.removeFrame()

	@Slot(QPoint)
	def showContextMenu(self, pos: QPoint):
		self.contextMenu.exec(self.mapToGlobal(pos))

	@Slot(str)
	def nameChanged(self, newName:str):
		self.direction.name = newName