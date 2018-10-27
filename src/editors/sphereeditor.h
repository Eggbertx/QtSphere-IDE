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
		int tabIndex;
		enum Type { None, SpritesetEditor, TextEditor };
		Type editorType();
	public slots:

	protected:
		QUndoStack* undoStack;
		Type type;

};

#endif // SPHEREEDITOR_H
