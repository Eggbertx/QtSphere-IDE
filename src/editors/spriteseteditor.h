#ifndef SPRITESETEDITOR_H
#define SPRITESETEDITOR_H

#include <QWidget>
#include "formats/spriteset.h"
#include "editors/sphereeditor.h"
#include "imagechooser.h"

namespace Ui {
	class SpritesetEditor;
}

class SpritesetEditor: public SphereEditor {
	Q_OBJECT

public:
	explicit SpritesetEditor(QWidget *parent = nullptr);
	~SpritesetEditor();
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
	Ui::SpritesetEditor *ui;
	ImageChooser* images;
};

#endif // SPRITESETEDITOR_H
