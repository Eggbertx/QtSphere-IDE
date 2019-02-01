#include <QUndoStack>

#include "widgets/sphereeditor.h"

SphereEditor::SphereEditor(QWidget* parent): QWidget(parent) {
	m_type = SphereEditor::None;
	m_tabIndex = -1;
	m_undoStack = new QUndoStack(this);
}

void SphereEditor::undo() {
	m_undoStack->undo();
}

void SphereEditor::redo() {
	m_undoStack->redo();
}

SphereEditor::Type SphereEditor::editorType() {
	return m_type;
}

int SphereEditor::getTabIndex() {
	return m_tabIndex;
}

void SphereEditor::setTabIndex(int index) {
	m_tabIndex = index;
}
