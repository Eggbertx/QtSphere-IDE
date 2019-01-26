#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QTextEdit>
#include "widgets/sphereeditor.h"

class TextEditor: public SphereEditor {
	public:
		TextEditor(QWidget* parent = nullptr);
		QTextEdit* textEditorWidget;
		void undo();
		void redo();

	private:

};

#endif // TEXTEDITOR_H
