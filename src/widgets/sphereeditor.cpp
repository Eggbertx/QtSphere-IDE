#include <QUndoStack>

#include "widgets/sphereeditor.h"

SphereEditor::SphereEditor(QWidget* parent): QWidget(parent) {
	this->type = SphereEditor::None;
	this->tabIndex = -1;
	this->undoStack = new QUndoStack(this);
}

void SphereEditor::undo() {
	this->undoStack->undo();
}

void SphereEditor::redo() {
	this->undoStack->redo();
}

SphereEditor::Type SphereEditor::editorType() {
	return this->type;
}
