#ifndef IMPORTOPTIONSDIALOG_H
#define IMPORTOPTIONSDIALOG_H

#include <QPushButton>
#include <QDialog>

namespace Ui {
	class ImportOptionsDialog;
}

class ImportOptionsDialog : public QDialog {
	Q_OBJECT

	public:
		explicit ImportOptionsDialog(QWidget *parent = 0);
		~ImportOptionsDialog();
		QSize getFrameSize();
		QColor getTransparencyIn();
		QColor getTransparencyOut();
		bool removeDuplicatesChecked();

	private slots:
		void on_colorOutButton_clicked();
		void on_colorInButton_clicked();
		void on_buttonBox_accepted();
		void on_buttonBox_rejected();

	private:
		void setButtonBackground(QPushButton *button, char* title);
		void setButtonBackground(QPushButton* button, QColor background);
		QColor getButtonBackground(QPushButton* button);
		Ui::ImportOptionsDialog *ui;
		QSize m_frameSize;
		QColor m_transparencyIn;
		QColor m_transparencyOut;
		bool m_removeDuplicates;
};

#endif // IMPORTOPTIONSDIALOG_H
