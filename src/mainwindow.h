#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QLabel>
#include <QMainWindow>
#include <QProcess>
#include <QWidget>
#include <QWidgetAction>

#include "qsiproject.h"
#include "dialogs/newmapdialog.h"
#include "formats/spherefile.h"
#include "widgets/soundplayer.h"
#include "widgets/sphereeditor.h"
#include "widgets/startpage.h"
#include "formats/qsitasklist.h"

namespace Ui {
	class MainWindow;
	extern MainWindow w;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
		explicit MainWindow(QWidget* parent = Q_NULLPTR);
		~MainWindow();
		QString getStatus();
		void setStatus(QString status);
		QString getTheme();
		void setTheme(QString theme = "default");
		void openFile(QString filename = "");
		void openProject(QString fileName);
		void setCurrentProject(QSIProject* project = nullptr);
		void closeProject();
		void saveCurrentTab();
		void prepareForImage();
		void prepareForMap();
		void prepareForTileSet();
		void prepareForSpriteset();
		void handleModifiedFiles();
		void refreshRecentFiles();
		SphereEditor* getCurrentEditor();
		void setEngine(QString which);
		static MainWindow* instance();
		int openProjectProperties(bool newFile = false, QSIProject* project = nullptr, QWidget *parent = nullptr);
		void playGame(QString gameDir);

	protected:
		void closeEvent(QCloseEvent* event);

	private slots:
		void nextTab();
		void prevTab();
		void checkCloseProjectOption();
		void onProjectLoaded(QSIProject* project);
		void onEngineDropdownChanged(int index);

		// File menu actions
		void onNewProjectActionTriggered();
		void onNewPlainTextFileActionTriggered();
		void onNewMapActionTriggered();
		// TODO: New spriteset, font, and windowstyle actions
		void onOpenProjectActionTriggered();
		void onOpenFileTriggered();
		void onClearRecentActionTriggered();
		void onImportTiledMapTriggered();
		void onImportImageToSpritesetTriggered();
		void onImportSystemFontToSphereFontTriggered();
		void onSaveTriggered();
		void onExitActionTriggered();

		// Edit menu actions
		void onUndoActionTriggered();
		void onRedoActionTriggered();
		// TODO: Cut, copy, paste, select all

		// View menu
		void onStartPageActionTriggered();
		void onProjectExplorerActionTriggered();
		void onProjectTaskListActionTriggered();
		void onSoundTestActionTriggered();

		// Project menu
		void onRefreshActionTriggered();
		void onCloseProjectTriggered();
		void onProjectPropertiesTriggered();

		// Tools menu
		void onConfigureQtSphereIDETriggered();

		// Help menu
		void onQtSphereIDEGithubActionTriggered();
		void onNSGithubActionTriggered();
		void onSphericalCommunityActionTriggered();
		void onAboutQtActionTriggered();
		void onAboutActionTriggered();

		void onTabCloseRequested(int index);
		void onNewTaskButtonClicked();
		void onDeleteTaskButtonClicked();
		void onTaskListTableContextMenuRequested(const QPoint &pos);
		void onTreeViewDoubleClicked(const QModelIndex &index);
		void onPlayGameTriggered();
		void onLegacyConfigActionTriggered();


	private:
		void setupEditors();
		void updateTreeView();
		bool validEngineDirCheck();
		void configureSphere();

		static MainWindow* _instance;
		Ui::MainWindow *ui;
		QString m_status;
		StartPage* m_startPage;
		QComboBox* m_engineSelector;
		QWidgetAction* m_engineSelectorAction;
		QLabel* m_statusLabel;
		bool m_projectLoaded;
		QSIProject* m_project;
		SoundPlayer* m_soundPlayer;
		QString m_theme;
		QSITaskList* m_taskList;
		NewMapDialog* m_newMapDialog;
		QFileSystemModel* m_fsModel;
		QStandardItemModel* m_emptyProjectModel;
};

#endif // MAINWINDOW_H
