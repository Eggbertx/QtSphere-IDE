#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileSystemModel>
#include <QLabel>
#include <QMainWindow>
#include <QProcess>
#include <QWidget>

#include "qsiproject.h"
#include "dialogs/newmapdialog.h"
#include "formats/spherefile.h"
#include "widgets/soundplayer.h"
#include "widgets/sphereeditor.h"
#include "widgets/startpage.h"

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

	protected:
		void closeEvent(QCloseEvent* event);

	private slots:
		void nextTab();
		void prevTab();
		void checkCloseProjectOption();
		void onProjectLoaded(QSIProject* project);
		void on_actionAbout_triggered();
		void on_actionAbout_Qt_triggered();
		void on_actionExit_triggered();
		void on_actionConfigure_QtSphere_IDE_triggered();
		void on_toolbarSaveButton_triggered();
		void on_toolbarOpenButton_triggered();
		void on_openFileTabs_tabCloseRequested(int index);
		void on_actionUndo_triggered();
		void on_actionRedo_triggered();
		void on_toolbarProjectProperties_triggered();
		void on_newProject_triggered();
		void on_actionProject_Properties_triggered();
		void on_newPlainTextFile_triggered();
		void on_actionOpenProject_triggered();
		void on_actionRefresh_triggered();
		void on_newTaskButton_clicked();
		void on_actionOpenFile_triggered();
		void on_actionQSIGithub_triggered();
		void on_actionMSGithub_triggered();
		void on_treeView_doubleClicked(const QModelIndex &index);
		void on_actionImage_to_Spriteset_triggered();
		void on_actionSave_triggered();
		void on_actionStart_Page_triggered();
		void on_taskListTable_customContextMenuRequested(const QPoint &pos);
		void on_delTaskButton_clicked();
		void on_actionSpherical_community_triggered();
		void on_actionClearRecent_triggered();
		void on_actionClose_Project_triggered();
		void on_actionClose_triggered();
		void on_toolbarPlayGame_triggered();
		void on_actionLegacyConfig_triggered();
		void on_newMap_triggered();
		void on_actionSystem_font_to_Sphere_font_triggered();
		void on_actionProject_Explorer_triggered();
		void on_actionProject_Task_List_triggered();
		void on_actionSound_Test_triggered();

	private:
		void setupEditors();
		void updateTreeView();
		bool validEngineDirCheck();
		static MainWindow* _instance;
		Ui::MainWindow *ui;
		QString m_status;
		StartPage* m_startPage;
		QLabel* m_statusLabel;
		bool m_projectLoaded;
		QSIProject* m_project;
		SoundPlayer* m_soundPlayer;
		QList<SphereEditor *> m_openEditors;
		QString m_theme;
		NewMapDialog* m_newMapDialog;
};

#endif // MAINWINDOW_H
