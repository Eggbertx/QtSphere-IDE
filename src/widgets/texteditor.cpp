#include <QFont>
#include <QTextEdit>
#include "widgets/texteditor.h"

TextEditor::TextEditor(QWidget *parent): SphereEditor(parent), QTextEdit (parent) {
	m_type = SphereEditor::TextEditor;
	QFont textFont("Monospace", 10);
	textFont.setFixedPitch(true);
	this->QTextEdit::setFont(textFont);
}
