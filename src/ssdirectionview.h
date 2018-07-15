#ifndef SSDIRECTIONVIEW_H
#define SSDIRECTIONVIEW_H

#include <QHBoxLayout>
#include <QGraphicsView>
#include <QImage>
#include <QLineEdit>
#include <QList>
#include <QToolButton>
#include <QWidget>


class SSDirectionView : public QFrame {
	Q_OBJECT
	public:
		explicit SSDirectionView(QString name = "", int numFrames = 1, QWidget *parent = 0);
		~SSDirectionView();
		QList<QImage*> getImages();
		QImage* getImage(int index);
		void setImages(QList<QImage*> images);
		void setImage(int index, QImage* image);
		QString getName();
		void setName(QString name);
		void addFrame(QImage* image);
		void removeFrame(int index);

	private slots:
		void showContextMenu(const QPoint&pos);
		void addFrameSlot();
		void removeFrameSlot();

	private:
		QWidget* framesContainer;
		QHBoxLayout* framesLayout;
		QLineEdit* nameLineEdit;
		QList<QGraphicsView*> frameViews;
		QToolButton* addFrameButton;
		QToolButton* removeFrameButton;

};

#endif // SSDIRECTIONVIEW_H
