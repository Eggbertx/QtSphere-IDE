#ifndef PROJECTPROPERTIESDIALOG_H
#define PROJECTPROPERTIESDIALOG_H

#include <QDialog>
#include "qsiproject.h"

namespace Ui {
	class ProjectPropertiesDialog;
}

class ProjectPropertiesDialog : public QDialog {
	Q_OBJECT
	public:
		explicit ProjectPropertiesDialog(bool newFile = false, QSIProject* project = 0, QWidget *parent = 0);
		~ProjectPropertiesDialog();
		QSIProject* getProject();

	private slots:
		void onResolutionCBoxCurrentTextChanged(const QString &newText);
		void onPathButtonClicked();
		void onButtonBoxAccepted();

private:
		Ui::ProjectPropertiesDialog *ui;
		bool m_isNew = false;
		QSIProject* m_project;
};

#endif // PROJECTPROPERTIESDIALOG_H
