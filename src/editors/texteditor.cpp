#include <QFont>
#include <QTextEdit>
#include "texteditor.h"

TextEditor::TextEditor(QWidget *parent): SphereEditor(parent) {
	this->type = SphereEditor::TextEditor;
	this->textEditorWidget = new QTextEdit(parent);
	QFont font("Monospace", 10);
	font.setFixedPitch(true);
	this->textEditorWidget->setFont(font);
}

void TextEditor::undo() {
	this->textEditorWidget->undo();
}

void TextEditor::redo() {
	this->textEditorWidget->redo();
}
