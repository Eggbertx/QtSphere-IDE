#ifndef ENTITYEDITDIALOG_H
#define ENTITYEDITDIALOG_H

#include <QDialog>

#include "formats/mapfile.h";

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
		void on_nameGenerate_btn_released();

	private:
		Ui::EntityEditDialog* ui;
		MapFile* map;
		bool attached;
		QString onCreateScript;
		QString onDestroyScript;
		QString onTouchScript;
		QString onTalkScript;
		QString onGenerateCommandsScript;
};

#endif // ENTITYEDITDIALOG_H
