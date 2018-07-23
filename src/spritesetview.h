#ifndef SPRITESETVIEW_H
#define SPRITESETVIEW_H

#include <QWidget>
#include "objects/spriteset.h"
#include "imagechooser.h"

namespace Ui {
	class SpritesetView;
}

class SpritesetView: public QWidget {
	Q_OBJECT

public:
	explicit SpritesetView(QWidget *parent = 0);
	~SpritesetView();
	void addDirection(QString name = "", int numFrames = 1);
	void addDirection(Spriteset::SSDirection direction);
	bool openFile(QString filename);
	bool attach(Spriteset* spriteset);
	void newFile();
	Spriteset* spriteset;

private slots:
	void showContextMenu(const QPoint &pos);

private:
	Ui::SpritesetView *ui;
	ImageChooser* images;

};

#endif // SPRITESETVIEW_H
