#ifndef IMAGECHOOSER_H
#define IMAGECHOOSER_H

#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QMenu>

class ImageChooser: public QWidget {
	Q_OBJECT

public:
	explicit ImageChooser(bool showTransparency, QWidget *parent = 0);
	~ImageChooser();
	QList<QImage> getImages();
	void setImages(QList<QImage> imglist);
	void setImages(QImage imgarr[], int num);
	int selectedIndex;

private:
	bool eventFilter(QObject* object, QEvent* event);
	QList<QImage> images;
	QMenu* rightClickMenu;
	QPoint mousePos;
	bool showTransparency;
	QPixmap alphabg;

};

#endif // IMAGECHOOSER_H
