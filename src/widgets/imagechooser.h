#ifndef IMAGECHOOSER_H
#define IMAGECHOOSER_H

#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QMenu>

class ImageChooser: public QWidget {
	Q_OBJECT

	public:
		explicit ImageChooser(bool showTransparency, QWidget *parent = nullptr);
		~ImageChooser();
		QList<QImage> getImages();
		void setImages(QList<QImage> imglist);
		void setImages(QImage imgarr[], int num);

	protected:
		bool eventFilter(QObject* object, QEvent* event);

	private:
		int m_selectedIndex;
		QList<QImage> m_images;
		QMenu* m_rightClickMenu;
		QPoint m_mousePos;
		bool m_showTransparency;
		QPixmap m_alphaBG;

};

#endif // IMAGECHOOSER_H
