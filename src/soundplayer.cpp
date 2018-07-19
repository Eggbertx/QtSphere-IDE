#include <QStyle>
#include <QFileInfo>
#include <QSizePolicy>
#include <QToolButton>
#include "soundplayer.h"
#include "ui_soundplayer.h"
#include "mainwindow.h"

SoundPlayer::SoundPlayer(QWidget *parent): QWidget(parent), ui(new Ui::SoundPlayer) {
	ui->setupUi(this);
	this->mediaPlayer = new QMediaPlayer(this);
	ui->playToggleBtn->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
	ui->stopBtn->setIcon(this->style()->standardIcon(QStyle::SP_MediaStop));
	ui->repeatBtn->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
	connect(this->mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this,
			SLOT(audioStateChanged(QMediaPlayer::State)));
	connect(this->mediaPlayer, SIGNAL(positionChanged(qint64)), this,
			SLOT(audioPositionChanged(qint64)));
	connect(this->mediaPlayer, SIGNAL(durationChanged(qint64)), this,
			SLOT(audioDurationChanged(qint64)));
}

void SoundPlayer::load(QString file) {
	QFileInfo fi = QFileInfo(file);
	this->mediaPlayer->stop();
	this->mediaPlayer->setMedia(QUrl::fromLocalFile(file));
	this->audioDuration = this->mediaPlayer->duration();
	ui->seekSlider->setEnabled(true);
	ui->seekSlider->setMinimum(0);
	ui->seekSlider->setMaximum(100);
	this->mediaPlayer->play();
	ui->currAudioLbl->setText(fi.fileName());
	ui->playToggleBtn->setIcon(this->style()->standardIcon(QStyle::SP_MediaPause));
}

void SoundPlayer::play() {
	this->mediaPlayer->play();
}

void SoundPlayer::stop() {
	this->mediaPlayer->stop();
}

SoundPlayer::~SoundPlayer() {
	disconnect(this->mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this,
			SLOT(audioStateChanged(QMediaPlayer::State)));
	disconnect(this->mediaPlayer, SIGNAL(positionChanged(qint64)), this,
			SLOT(audioPositionChanged(qint64)));
	disconnect(this->mediaPlayer, SIGNAL(durationChanged(qint64)), this,
			SLOT(audioDurationChanged(qint64)));
	delete ui;
}

void SoundPlayer::on_playToggleBtn_clicked() {
	switch(this->mediaPlayer->state()) {
		case QMediaPlayer::StoppedState: {
			this->mediaPlayer->setPosition(0);
			this->mediaPlayer->play();
			break;
		}
		case QMediaPlayer::PlayingState:
			this->mediaPlayer->pause();
			break;
		case QMediaPlayer::PausedState:
			this->mediaPlayer->play();
			break;
	}
}

void SoundPlayer::on_stopBtn_clicked() {
	this->mediaPlayer->stop();
	ui->playToggleBtn->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
}

void SoundPlayer::on_repeatBtn_clicked(bool checked) {
	this->loopingAudio = checked;
}

void SoundPlayer::audioDurationChanged(qint64 duration) {
	this->audioDuration = duration;
}

void SoundPlayer::audioPositionChanged(qint64 position) {
	float floatPos = (float)position;
	int percent = floatPos/this->audioDuration*100;
	if(!ui->seekSlider->isSliderDown()) ui->seekSlider->setSliderPosition(percent);
}

void SoundPlayer::audioStateChanged(QMediaPlayer::State state) {
	switch(state) {
		case QMediaPlayer::PlayingState:
			ui->playToggleBtn->setIcon(this->style()->standardIcon(QStyle::SP_MediaPause));
			break;
		case QMediaPlayer::PausedState:
			ui->playToggleBtn->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
			break;
		case QMediaPlayer::StoppedState: {
			ui->playToggleBtn->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
			if(this->mediaPlayer->position() == this->audioDuration && this->loopingAudio) {
				this->mediaPlayer->play();
			} else ui->seekSlider->setSliderPosition(0);
			break;
		}
	}
}

void SoundPlayer::on_seekSlider_valueChanged(int value) {
	if(!ui->seekSlider->isSliderDown()) return;
	qint64 newPos = 0;
	float floatPos = (float)value;
	newPos = this->audioDuration * (floatPos/100);
	this->mediaPlayer->setPosition(newPos);
}
