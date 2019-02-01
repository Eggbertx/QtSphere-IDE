#ifndef SPHEREEDITOR_H
#define SPHEREEDITOR_H

#include <QWidget>
#include <QUndoStack>

class SphereEditor : public QWidget {
	Q_OBJECT
	Q_ENUMS(Type)
	public:
		explicit SphereEditor(QWidget* parent = nullptr);
		void undo();
		void redo();
		enum Type { None, SpritesetEditor, TextEditor };
		Type editorType();
		int getTabIndex();
		void setTabIndex(int index);

	public slots:

	protected:
		QUndoStack* m_undoStack;
		Type m_type;
		int m_tabIndex;
};

#endif // SPHEREEDITOR_H
