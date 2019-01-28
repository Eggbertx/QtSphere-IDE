#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QFileSystemModel>
#include <QLabel>
#include <QWidget>
#include <QVariant>
#include <QComboBox>

#include "widgets/sphereeditor.h"
#include "formats/spherefile.h"
#include "qsiproject.h"
#include "widgets/soundplayer.h"
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
		void saveCurrentTab();
		void prepareForImage();
		void prepareForMap();
		void prepareForTileSet();
		void prepareForSpriteset();
		void handleModifiedFiles();
		void refreshRecentFiles();
		void closeEvent(QCloseEvent* event);
		SphereEditor* getCurrentEditor();
		static MainWindow* instance();

	private slots:
		void nextTab();
		void prevTab();
		void checkCloseProjectOption();
		void on_actionAbout_triggered();
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

	private:
		static MainWindow* _instance;
		Ui::MainWindow *ui;
		QString status;
		QLabel* statusLabel;
		bool projectLoaded;
		QSIProject* project;
		SoundPlayer* soundPlayer;
		QList<SphereEditor *> openEditors;
		QString theme;
		void setupEditors();
		void updateTreeView();
};

#endif // MAINWINDOW_H
