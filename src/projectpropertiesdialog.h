#ifndef PROJECTPROPERTIESDIALOG_H
#define PROJECTPROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
	class ProjectPropertiesDialog;
}

class ProjectPropertiesDialog : public QDialog {
	Q_OBJECT
	public:
        explicit ProjectPropertiesDialog(bool newFile = false, QWidget *parent = 0);
        ~ProjectPropertiesDialog();

private slots:
    void on_pathButton_triggered(QAction *arg1);

    void on_resolutionCBox_currentTextChanged(const QString &newText);

private:
		Ui::ProjectPropertiesDialog *ui;
        bool isNew = false;
};

#endif // PROJECTPROPERTIESDIALOG_H
