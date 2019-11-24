#ifndef FONTEDITOR_H
#define FONTEDITOR_H

#include <QWidget>

namespace Ui {
	class FontEditor;
}

class FontEditor : public QWidget {
	Q_OBJECT

public:
	explicit FontEditor(QWidget *parent = nullptr);
	~FontEditor();

private:
	Ui::FontEditor* ui;
};

#endif // FONTEDITOR_H
