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
		bool writeSGMFile();

	private slots:
		void on_resolutionCBox_currentTextChanged(const QString &newText);
		void on_pathButton_clicked();
		void on_buttonBox_accepted();

private:
		Ui::ProjectPropertiesDialog *ui;
		bool isNew = false;
		QSIProject* project;
};

#endif // PROJECTPROPERTIESDIALOG_H
