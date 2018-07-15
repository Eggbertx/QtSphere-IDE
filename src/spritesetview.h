#ifndef SPRITESETVIEW_H
#define SPRITESETVIEW_H

#include <QWidget>

namespace Ui {
	class SpritesetView;
}

class SpritesetView: public QWidget {
	Q_OBJECT

public:
	explicit SpritesetView(QWidget *parent = 0);
	~SpritesetView();
	void addDirection(QString name = "", int numFrames = 1);

private slots:
	void showContextMenu(const QPoint &pos);

private:
	Ui::SpritesetView *ui;
};

#endif // SPRITESETVIEW_H
