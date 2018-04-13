#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

namespace Ui {
	class SettingsWindow;
	extern SettingsWindow sw;
}

class SettingsWindow : public QDialog {
	Q_OBJECT

public:
	explicit SettingsWindow(QWidget *parent = 0);
	~SettingsWindow();
	QString legacyDir;

private slots:
	void on_cancelButton_clicked();

	void on_legacyDir_btn_clicked();

private:
	Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
