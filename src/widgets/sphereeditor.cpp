#include <QUndoStack>

#include "widgets/sphereeditor.h"

SphereEditor::SphereEditor(QWidget* parent): QWidget(parent) {
	m_type = SphereEditor::None;
	m_tabIndex = -1;
	m_undoStack = new QUndoStack(this);
	m_undoAction = new QAction("&Undo");
	m_undoAction->setShortcuts(QKeySequence::Undo);
	m_redoAction = new QAction("&Redo");
	m_redoAction->setShortcuts(QKeySequence::Redo);
	m_attachedFile = nullptr;
}

SphereEditor::~SphereEditor() {
	close();
	delete m_undoStack;
	delete m_undoAction;
	delete m_redoAction;
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

void SphereEditor::closeFile() {
	if(m_attachedFile != nullptr) m_attachedFile->close();
}
