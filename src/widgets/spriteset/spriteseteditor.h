#ifndef SPRITESETEDITOR_H
#define SPRITESETEDITOR_H

#include <QWidget>
#include "formats/spriteset.h"
#include "widgets/sphereeditor.h"
#include "widgets/imagechooser.h"

namespace Ui {
	class SpritesetEditor;
}

class SpritesetEditor: public SphereEditor {
	Q_OBJECT

public:
	explicit SpritesetEditor(QWidget *parent = nullptr);
	~SpritesetEditor();
	void addDirection(QString name = "", int numFrames = 1);
	void attachDirection(int index);
	bool openFile(QString filename);

	bool attach(Spriteset* spriteset);
	Spriteset* getSpriteset();
	void newFile();


private slots:
	void showContextMenu(const QPoint &pos);
	void on_animDirChoose_currentIndexChanged(int index);

private:
	Ui::SpritesetEditor *ui;
	ImageChooser* m_images;
	Spriteset* m_spriteset;
};

#endif // SPRITESETEDITOR_H
