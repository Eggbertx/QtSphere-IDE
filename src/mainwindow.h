#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QFileSystemModel>
#include <QLabel>
#include <QWidget>
#include <QVariant>
#include <QComboBox>

#include "objects/spherefile.h"
#include "qsiproject.h"
#include "soundplayer.h"
#include "startpage.h"

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
		void openFile(QString fileName = "");
		void addWidgetTab(QWidget* widget, QString tabname);
		void openProject(QString fileName);
		void refreshProjectList();
		void saveCurrentTab();
		void prepareForImage();
		void prepareForMap();
		void prepareForTileSet();
		void prepareForSpriteset();
		void handleModifiedFiles();
		void closeEvent(QCloseEvent* event);
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

	private:
		static MainWindow* _instance;
		Ui::MainWindow *ui;
		QString status;
		QLabel* statusLabel;
		QSIProject* project;
		SoundPlayer* soundPlayer;
		QList<QWidget *> openFiles;
		QString theme;
		void setupEditors();
		void setupTextEditor(QTextEdit *editor);
		void setupTreeView();
};

#endif // MAINWINDOW_H
