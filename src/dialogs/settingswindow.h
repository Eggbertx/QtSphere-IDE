#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
	class SettingsWindow;
	extern SettingsWindow sw;
}

class SettingsWindow : public QDialog {
	Q_OBJECT

	public:
		explicit SettingsWindow(QWidget *parent = 0);
		~SettingsWindow();


	private slots:
		void on_cancelButton_clicked();
		void on_legacyDir_btn_clicked();
		void on_themeCombo_currentIndexChanged(const QString &newtext);
		void on_okButton_clicked();
		void on_applyButton_clicked();
		void on_addDirButton_clicked();
		void on_removeDirButton_clicked();
		void mapCursorColChanged(QColor color);

	private:
		void saveSettings();
		Ui::SettingsWindow *ui;
		QString m_currentTheme;
		QString m_newTheme;
		QString m_mapCursorColor;
		QString m_legacyDir;
};

#endif // SETTINGSWINDOW_H
