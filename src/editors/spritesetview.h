#ifndef SPRITESETVIEW_H
#define SPRITESETVIEW_H

#include <QWidget>
#include "formats/spriteset.h"
#include "editors/sphereeditor.h"
#include "imagechooser.h"

namespace Ui {
	class SpritesetView;
}

class SpritesetView: public SphereEditor {
	Q_OBJECT

public:
	explicit SpritesetView(QWidget *parent = nullptr);
	~SpritesetView();
	void addDirection(QString name = "", int numFrames = 1);
	void addDirection(Spriteset::SSDirection direction);
	bool openFile(QString filename);
	bool attach(Spriteset* spriteset);
	void newFile();
	Spriteset* spriteset;

private slots:
	void showContextMenu(const QPoint &pos);
	void on_animDirChoose_currentIndexChanged(int index);

private:
	Ui::SpritesetView *ui;
	ImageChooser* images;
};

#endif // SPRITESETVIEW_H
