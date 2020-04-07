#ifndef SPHEREEDITOR_H
#define SPHEREEDITOR_H

#include <QAction>
#include <QUndoStack>
#include <QWidget>

#include "formats/spherefile.h"

class SphereEditor : public QWidget {
	Q_OBJECT
	Q_ENUMS(Type)
	public:
		explicit SphereEditor(QWidget* parent = nullptr);
		~SphereEditor();
		void undo();
		void redo();
		enum Type { None, MapEditor, SpritesetEditor, TextEditor };
		Type editorType();
		int getTabIndex();
		void setTabIndex(int index);
		virtual void closeFile();

	public slots:

	protected:
		QUndoStack* m_undoStack;
		QAction* m_undoAction;
		QAction* m_redoAction;
		SphereFile* m_attachedFile;
		Type m_type;
		int m_tabIndex;
};

#endif // SPHEREEDITOR_H
