#include <QStyle>
#include <QFileInfo>
#include <QSizePolicy>
#include <QToolButton>
#include "widgets/soundplayer.h"
#include "ui_soundplayer.h"
#include "mainwindow.h"

SoundPlayer::SoundPlayer(QWidget *parent): QWidget(parent), ui(new Ui::SoundPlayer) {
	ui->setupUi(this);
	m_mediaPlayer = new QMediaPlayer(this);
	ui->playToggleBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	ui->stopBtn->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	ui->repeatBtn->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
	connect(m_mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)),
			this, SLOT(audioStateChanged(QMediaPlayer::State)));
	connect(m_mediaPlayer, SIGNAL(positionChanged(qint64)),
			this, SLOT(audioPositionChanged(qint64)));
	connect(m_mediaPlayer, SIGNAL(durationChanged(qint64)),
			this, SLOT(audioDurationChanged(qint64)));
}

void SoundPlayer::load(QString file) {
	QFileInfo fi = QFileInfo(file);
	m_mediaPlayer->stop();
	m_mediaPlayer->setMedia(QUrl::fromLocalFile(file));
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

SoundPlayer::~SoundPlayer() {
	disconnect(m_mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)),
				this, SLOT(audioStateChanged(QMediaPlayer::State)));
	disconnect(m_mediaPlayer, SIGNAL(positionChanged(qint64)),
				this, SLOT(audioPositionChanged(qint64)));
	disconnect(m_mediaPlayer, SIGNAL(durationChanged(qint64)),
				this, SLOT(audioDurationChanged(qint64)));
	delete ui;
}

void SoundPlayer::on_playToggleBtn_clicked() {
	switch(m_mediaPlayer->state()) {
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

void SoundPlayer::on_stopBtn_clicked() {
	m_mediaPlayer->stop();
	ui->playToggleBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
}

void SoundPlayer::on_repeatBtn_clicked(bool checked) {
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

void SoundPlayer::audioStateChanged(QMediaPlayer::State state) {
	switch(state) {
		case QMediaPlayer::PlayingState:
			ui->playToggleBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
			break;
		case QMediaPlayer::PausedState:
			ui->playToggleBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
			break;
		case QMediaPlayer::StoppedState: {
			ui->playToggleBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
			if(m_mediaPlayer->position() == m_audioDuration && m_loopingAudio) {
				m_mediaPlayer->play();
			} else ui->seekSlider->setSliderPosition(0);
			break;
		}
	}
}

void SoundPlayer::on_seekSlider_valueChanged(int value) {
	if(!ui->seekSlider->isSliderDown()) return;
	qint64 newPos = 0;
	float floatPos = (float)value;
	newPos = m_audioDuration * (floatPos/100);
	m_mediaPlayer->setPosition(newPos);
}
