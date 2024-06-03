#include <QAudioOutput>
#include <QStyle>
#include <QFileInfo>
#include <QSizePolicy>
#include <QToolButton>
#include "widgets/soundplayer.h"
#include "ui_soundplayer.h"

SoundPlayer::SoundPlayer(QWidget *parent): QWidget(parent), ui(new Ui::SoundPlayer) {
	ui->setupUi(this);

	m_mediaPlayer = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput();
    m_audioOutput->setVolume(100);
    m_mediaPlayer->setAudioOutput(m_audioOutput);

	ui->playToggleBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	ui->stopBtn->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	ui->repeatBtn->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

	connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &SoundPlayer::audioStateChanged);
	connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &SoundPlayer::audioPositionChanged);
	connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &SoundPlayer::audioDurationChanged);
	connect(ui->playToggleBtn, &QToolButton::clicked, this, &SoundPlayer::onPlayButtonClicked);
	connect(ui->stopBtn, &QToolButton::clicked, this, &SoundPlayer::onStopButtonClicked);
	connect(ui->repeatBtn, &QToolButton::clicked, this, &SoundPlayer::onRepeatButtonClicked);
	connect(ui->seekSlider, &QSlider::valueChanged, this, &SoundPlayer::onSeekSliderValueChanged);
}

SoundPlayer::~SoundPlayer() {
	disconnect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this, &SoundPlayer::audioStateChanged);
	disconnect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &SoundPlayer::audioPositionChanged);
	disconnect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &SoundPlayer::audioDurationChanged);
	disconnect(ui->playToggleBtn, &QToolButton::clicked, this, &SoundPlayer::onPlayButtonClicked);
	disconnect(ui->stopBtn, &QToolButton::clicked, this, &SoundPlayer::onStopButtonClicked);
	disconnect(ui->repeatBtn, &QToolButton::clicked, this, &SoundPlayer::onRepeatButtonClicked);
	disconnect(ui->seekSlider, &QSlider::valueChanged, this, &SoundPlayer::onSeekSliderValueChanged);
    delete m_mediaPlayer;
    delete m_audioOutput;
	delete ui;
}

void SoundPlayer::load(QString file) {
	QFileInfo fi = QFileInfo(file);
	m_mediaPlayer->stop();
    m_mediaPlayer->setSource(QUrl::fromLocalFile(file));
	m_audioDuration = m_mediaPlayer->duration();
	ui->seekSlider->setEnabled(true);
	ui->seekSlider->setMinimum(0);
	ui->seekSlider->setMaximum(100);
	m_mediaPlayer->play();
	ui->currAudioLbl->setText(fi.fileName());
	ui->playToggleBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
}

void SoundPlayer::play() {
	m_mediaPlayer->play();
}

void SoundPlayer::stop() {
	m_mediaPlayer->stop();
}

void SoundPlayer::onPlayButtonClicked() {
	switch(m_mediaPlayer->playbackState()) {
	case QMediaPlayer::StoppedState: {
		m_mediaPlayer->setPosition(0);
		m_mediaPlayer->play();
		break;
	}
	case QMediaPlayer::PlayingState:
		m_mediaPlayer->pause();
		break;
	case QMediaPlayer::PausedState:
		m_mediaPlayer->play();
		break;
	}
}

void SoundPlayer::onStopButtonClicked() {
    qDebug() << "Stop button clicked";
	m_mediaPlayer->stop();
	ui->playToggleBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
}

void SoundPlayer::onRepeatButtonClicked(bool checked) {
	m_loopingAudio = checked;
}

void SoundPlayer::audioDurationChanged(qint64 duration) {
	m_audioDuration = duration;
}

void SoundPlayer::audioPositionChanged(qint64 position) {
	float floatPos = (float)position;
	int percent = floatPos/m_audioDuration*100;
	if(!ui->seekSlider->isSliderDown()) ui->seekSlider->setSliderPosition(percent);
}

void SoundPlayer::audioStateChanged(QMediaPlayer::PlaybackState state) {
	switch(state) {
	case QMediaPlayer::PlayingState:
        qDebug() << "Playing";
		ui->playToggleBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
		break;
	case QMediaPlayer::PausedState:
        qDebug() << "Paused";
		ui->playToggleBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
		break;
	case QMediaPlayer::StoppedState: {
        qDebug() << "Stopped";
		ui->playToggleBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        if(m_mediaPlayer->position() == m_audioDuration && m_loopingAudio) {
			m_mediaPlayer->play();
        } else ui->seekSlider->setSliderPosition(0);
		break;
        }
	}
}

void SoundPlayer::onSeekSliderValueChanged(int value) {
	if(!ui->seekSlider->isSliderDown()) return;
    qDebug() << "Position changed to " << value;
    qint64 newPos = 0;
	float floatPos = (float)value;
	newPos = m_audioDuration * (floatPos/100);
	m_mediaPlayer->setPosition(newPos);
}
