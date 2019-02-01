#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QTextEdit>
#include "widgets/sphereeditor.h"

class TextEditor: public SphereEditor, public QTextEdit {
	public:
		TextEditor(QWidget* parent = nullptr);

	private:

};

#endif // TEXTEDITOR_H
