#ifndef PIXMAPFRAME_H
#define PIXMAPFRAME_H

#include <QWidget>
#include <QGraphicsView>

class AnimatablePixmap : public QGraphicsView {
	Q_OBJECT
	public:
		explicit AnimatablePixmap(bool showTransparency = true, QWidget *parent = nullptr);
		explicit AnimatablePixmap(QPixmap* singlePixmap, bool showTransparency = true, QWidget* parent = nullptr);
		static struct PixmapFrame {
			QPixmap* pixmap;
			int delay;
		}frame_struct;
		bool selected;

	signals:

	public slots:
		int nextFrame();

	private:
		void init(bool showTransparency);
		void setFrames(QList<PixmapFrame*> images);
		int addFrame(QPixmap* pixmap, int delay);
		QPixmap* getImage(int index);
		int getDelay(int index);
		void setImage(int index, QPixmap* pixmap);
		void setDelay(int index, int delay);
		void setFrame(int index, PixmapFrame* frame);
		PixmapFrame getFrame(int index);
		QTimer* timer;
		bool showTransparency;
		QList<PixmapFrame*> frames;
};

#endif // PIXMAPFRAME_H
