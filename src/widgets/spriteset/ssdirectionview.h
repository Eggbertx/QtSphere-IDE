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
		explicit SSDirectionView(Spriteset* spriteset, int index, QWidget* parent = nullptr);
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

	protected:
		void mousePressEvent(QMouseEvent* event);

	private slots:
		void showContextMenu(const QPoint&pos);
		void addFrameSlot();
		void removeFrameSlot();
		void changeDirectionName(QString name);

	private:
		QWidget* m_framesContainer;
		QHBoxLayout* m_framesLayout;
		QLineEdit* m_nameLineEdit;
		QList<QGraphicsView*> m_frameViews;
		QToolButton* m_addFrameButton;
		QToolButton* m_removeFrameButton;
		Spriteset* m_spriteset;
		Spriteset::SSDirection* m_direction;
		int m_directionIndex;
		int m_zoomFactor;

};

#endif // SSDIRECTIONVIEW_H
