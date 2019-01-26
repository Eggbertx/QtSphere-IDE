#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QWidget>
#include <QMediaPlayer>

namespace Ui {
	class SoundPlayer;
}

class SoundPlayer : public QWidget {
	Q_OBJECT

	public:
		explicit SoundPlayer(QWidget *parent = 0);
		void load(QString file);
		void play();
		void stop();
		~SoundPlayer();

	private slots:
		void audioStateChanged(QMediaPlayer::State state);
		void audioPositionChanged(qint64 position);
		void audioDurationChanged(qint64 duration);
		void on_playToggleBtn_clicked();
		void on_stopBtn_clicked();
		void on_repeatBtn_clicked(bool checked);
		void on_seekSlider_valueChanged(int value);

	private:
		Ui::SoundPlayer *ui;
		QMediaPlayer *mediaPlayer;
		bool loopingAudio = false;
		qint64 audioDuration;

};

#endif // SOUNDPLAYER_H
