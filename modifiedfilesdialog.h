#ifndef MODIFIEDFILESDIALOG_H
#define MODIFIEDFILESDIALOG_H

#include <QDialog>
#include <objects/qsifile.h>
namespace Ui {
	class ModifiedFilesDialog;
}

class ModifiedFilesDialog : public QDialog {
    Q_OBJECT

	public:
		explicit ModifiedFilesDialog(QWidget *parent = 0);
		~ModifiedFilesDialog();
		int getResult();
        void addModifiedItem(QSIFile *item);
        void saveModifiedItem(QSIFile item);
        void discardModifiedItem(QSIFile item);

	private slots:
		void on_cancelButton_clicked();
		void on_discardAllButton_2_clicked();
		void on_saveAllButton_clicked();

	private:
		Ui::ModifiedFilesDialog *ui;
		int result; // 0 == cancel, 1 = discard all, 2 = save all

};

#endif // MODIFIEDFILESDIALOG_H
