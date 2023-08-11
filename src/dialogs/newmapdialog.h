#ifndef NEWMAPDIALOG_H
#define NEWMAPDIALOG_H

#include <QDialog>
#include "qsiproject.h"

namespace Ui {
	class NewMapDialog;
}

class NewMapDialog : public QDialog {
	Q_OBJECT

	public:
		explicit NewMapDialog(QSIProject* project = nullptr, QWidget *parent = nullptr);
		~NewMapDialog();
		QSize mapSize();
		QString tilesetPath();
		void setTilesetPath(QString path);

	private slots:
		void onBrowseButtonClicked();

private:
		Ui::NewMapDialog *ui;
		QSIProject* m_project;

};

#endif // NEWMAPDIALOG_H
