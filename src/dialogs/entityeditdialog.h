#ifndef ENTITYEDITDIALOG_H
#define ENTITYEDITDIALOG_H

#include <QDialog>

#include "formats/mapfile.h"

namespace Ui {
	class EntityEditDialog;
}

class EntityEditDialog : public QDialog {
	Q_OBJECT

	public:
		explicit EntityEditDialog(QWidget* parent = nullptr);
		~EntityEditDialog();
		QString getScript(int which);
		void attachMap(MapFile* map);

	private slots:
		void onNameGenerateBtnReleased();

	private:
		Ui::EntityEditDialog* ui;
		MapFile* m_map;
		bool m_attached;
		QString m_onCreateScript;
		QString m_onDestroyScript;
		QString m_onTouchScript;
		QString m_onTalkScript;
		QString m_onGenerateCommandsScript;
};

#endif // ENTITYEDITDIALOG_H
