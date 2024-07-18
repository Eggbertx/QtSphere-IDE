from os import path

from PySide6.QtCore import Qt, Slot, QUrl
from PySide6.QtWidgets import QWidget, QStyle
from PySide6.QtMultimedia import QMediaPlayer, QAudioOutput

from ui.ui_soundplayer import Ui_SoundPlayer

class SoundPlayer(QWidget):
	ui: Ui_SoundPlayer
	mediaPlayer:QMediaPlayer
	audioOutput:QAudioOutput
	loopingAudio:bool
	audioDuration:int

	def __init__(self, parent: QWidget = None):
		super().__init__(parent)
		self.ui = Ui_SoundPlayer()
		self.ui.setupUi(self)
		self.mediaPlayer = QMediaPlayer(self)
		self.audioOutput = QAudioOutput()
		self.audioOutput.setVolume(100)
		self.mediaPlayer.setAudioOutput(self.audioOutput)
		self.loopingAudio = False

		self.ui.playToggleBtn.clicked.connect(self.onPlayButtonClicked)
		self.ui.repeatBtn.clicked.connect(self.onRepeatButtonClicked)
		self.ui.stopBtn.clicked.connect(self.onStopButtonClicked)
		self.mediaPlayer.durationChanged.connect(self.onAudioDurationChanged)
		self.mediaPlayer.positionChanged.connect(self.onAudioPositionChanged)
		self.mediaPlayer.playbackStateChanged.connect(self.onPlaybackStateChanged)
		self.ui.seekSlider.valueChanged.connect(self.onSeekSliderValueChanged)


	def load(self, file:str):
		self.mediaPlayer.stop()
		self.mediaPlayer.setSource(QUrl.fromLocalFile(file))
		self.audioDuration = self.mediaPlayer.duration()
		self.ui.seekSlider.setEnabled(True)
		self.ui.seekSlider.setMinimum(0)
		self.ui.seekSlider.setMaximum(100)
		self.ui.currAudioLbl.setText(path.basename(file))
		self.play()


	def play(self):
		self.mediaPlayer.play()
		self.ui.playToggleBtn.setIcon(self.style().standardIcon(QStyle.StandardPixmap.SP_MediaPause))


	def pause(self):
		self.mediaPlayer.pause()
		self.ui.playToggleBtn.setIcon(self.style().standardIcon(QStyle.StandardPixmap.SP_MediaPlay))


	def stop(self):
		self.mediaPlayer.stop()
		self.ui.playToggleBtn.setIcon(self.style().standardIcon(QStyle.StandardPixmap.SP_MediaPlay))


	@Slot()
	def onPlayButtonClicked(self):
		match self.mediaPlayer.playbackState():
			case QMediaPlayer.PlaybackState.PlayingState:
				self.pause()
			case QMediaPlayer.PlaybackState.PausedState:
				self.play()
			case QMediaPlayer.PlaybackState.StoppedState:
				self.mediaPlayer.setPosition(0)
				self.play()


	@Slot()
	def onRepeatButtonClicked(self):
		self.loopingAudio = self.ui.repeatBtn.isChecked()


	@Slot()
	def onStopButtonClicked(self):
		self.stop()


	@Slot(int)
	def onAudioDurationChanged(self, duration:int):
		self.audioDuration = duration


	@Slot(int)
	def onAudioPositionChanged(self, position:int):
		floatPos = float(position)
		percent = floatPos / self.audioDuration * 100
		if not self.ui.seekSlider.isSliderDown():
			self.ui.seekSlider.setSliderPosition(percent)


	@Slot(QMediaPlayer.PlaybackState)
	def onPlaybackStateChanged(self, state:QMediaPlayer.PlaybackState):
		match state:
			case QMediaPlayer.PlayingState:
				self.ui.playToggleBtn.setIcon(self.style().standardIcon(QStyle.StandardPixmap.SP_MediaPause))
			case QMediaPlayer.PausedState:
				self.ui.playToggleBtn.setIcon(self.style().standardIcon(QStyle.StandardPixmap.SP_MediaPlay))
			case QMediaPlayer.StoppedState:
				self.ui.playToggleBtn.setIcon(self.style().standardIcon(QStyle.StandardPixmap.SP_MediaPlay))
				if self.mediaPlayer.position() == self.audioDuration and self.loopingAudio:
					self.mediaPlayer.play()
				else:
					self.ui.seekSlider.setSliderPosition(0)


	@Slot(int)
	def onSeekSliderValueChanged(self, value:int):
		if not self.ui.seekSlider.isSliderDown():
			return
		newPos = int(self.audioDuration * (value / 100))
		self.mediaPlayer.setPosition(newPos)
