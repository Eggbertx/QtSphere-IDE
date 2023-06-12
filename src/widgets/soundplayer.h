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
		~SoundPlayer();
		void load(QString file);
		void play();
		void stop();

	private slots:
		void audioStateChanged(QMediaPlayer::PlaybackState state);
		void audioPositionChanged(qint64 position);
		void audioDurationChanged(qint64 duration);
		void on_playToggleBtn_clicked();
		void on_stopBtn_clicked();
		void on_repeatBtn_clicked(bool checked);
		void on_seekSlider_valueChanged(int value);

	private:
		Ui::SoundPlayer *ui;
		QMediaPlayer* m_mediaPlayer;
		bool m_loopingAudio = false;
		qint64 m_audioDuration;

};

#endif // SOUNDPLAYER_H
