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
		void mapCursorColChanged(QColor color);
		void gridColorChanged(QColor color);

		void onNeosphereDirButtonClicked();
		void onLegacySphereDirButtonClicked();
		void onThemeComboCurrentTextChanged(const QString &newtext);
		void onAddDirButtonClicked(); //
		void onRemoveDirButtonClicked();
		void onBrowseDirButtonClicked();

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
