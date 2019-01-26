#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QWidget>
#include <QToolButton>

// sort of similar to the ColorButton from the Tiled map editor
class ColorButton : public QToolButton {
	Q_OBJECT
	public:
		explicit ColorButton(QWidget* parent = nullptr, QColor color = Qt::white);
		void setColor(QColor color);
		QColor getColor();
	protected:
		void mouseReleaseEvent(QMouseEvent* e) override;

	signals:
		void colorChanged(QColor color);

	private:
		void updateIcon();
		QSize pixmapSize;
		QPixmap colorIcon;
		QColor color;
};

#endif // COLORBUTTON_H
