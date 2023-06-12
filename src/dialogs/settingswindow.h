#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QAbstractButton>
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
		void onOk();
		void onApply();
		void onCancel();
		void on_neosphereDir_btn_clicked();
		void on_legacySphereDir_btn_clicked();
		void on_themeCombo_currentIndexChanged(const QString &newtext);
		void on_addDirButton_clicked();
		void on_removeDirButton_clicked();
		void mapCursorColChanged(QColor color);
		void gridColorChanged(QColor color);
		void on_browseDirButton_clicked();

	private:
		void saveSettings();
		Ui::SettingsWindow *ui;
		QString m_currentTheme;
		QString m_newTheme;
		QString m_mapCursorColor;
		QString m_gridColor;
		QString m_neosphereDir;
		QString m_legacySphereDir;
};

#endif // SETTINGSWINDOW_H
