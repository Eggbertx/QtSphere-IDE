#ifndef SSDIRECTIONVIEW_H
#define SSDIRECTIONVIEW_H

#include <QHBoxLayout>
#include <QGraphicsView>
#include <QImage>
#include <QLineEdit>
#include <QList>
#include <QToolButton>
#include <QWidget>

#include "formats/spriteset.h"

class SSDirectionView : public QFrame {
	Q_OBJECT
	public:
		explicit SSDirectionView(QString name = "", int numFrames = 1, Spriteset::SSDirection* ssDirection = nullptr, QWidget *parent = 0);
		~SSDirectionView();
		QList<QImage*> getImages();
		QImage* getImage(int index);
		void setImages(QList<QImage*> images);
		void setImage(int index, QImage image);
		QString getName();
		void setName(QString name);
		void addFrame(QImage* image);
		void removeFrame(int index);
		void setZoom(int factor = 2);
		int getZoom();

	private slots:
		void showContextMenu(const QPoint&pos);
		void addFrameSlot();
		void removeFrameSlot();
		void changeDirectionName(QString name);

	private:
		void mousePressEvent(QMouseEvent* event);
		QWidget* framesContainer;
		QHBoxLayout* framesLayout;
		QLineEdit* nameLineEdit;
		QList<QGraphicsView*> frameViews;
		QToolButton* addFrameButton;
		QToolButton* removeFrameButton;
		Spriteset* spriteset;
		Spriteset::SSDirection* ssDirection;
		int zoomFactor;

};

#endif // SSDIRECTIONVIEW_H
