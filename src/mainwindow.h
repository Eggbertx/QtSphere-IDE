#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QFileSystemModel>
#include <QLabel>
#include <QWidget>
#include <QVariant>
#include <QComboBox>

#include "config.h"
#include "objects/spherefile.h"
#include "qsiproject.h"
#include "soundplayer.h"

#define MAINWINDOW_TITLE TARGET " " VERSION

namespace Ui {
	class MainWindow;
	extern MainWindow w;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
		explicit MainWindow(Config* config = Q_NULLPTR, QWidget *parent = Q_NULLPTR);
		~MainWindow();
		QString getStatus();
		void setStatus(QString status);
		void openFile(QString fileName = "");
		void addWidgetTab(QWidget* widget, QString tabname);
		void openProject(QString fileName, bool reopen = false);
		void saveCurrentTab();
		void prepareForImage();
		void prepareForMap();
		void prepareForTileSet();
		void prepareForSpriteset();
		void handleModifiedFiles();
		void readSSProj();
		void readProjectFile(QString filename);
		void readGameJSON();

		void console(QVariant s, int which = 0);
		void closeEvent(QCloseEvent* event);
		static MainWindow* instance();
		Config* config;

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
		void showContextMenu(const QPoint& pos);
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

	private:
		static MainWindow* _instance;
		Ui::MainWindow *ui;
		QString status;
		QLabel* statusLabel;
		QString projectDir;
		QSIProject* project;
		SoundPlayer* soundPlayer;
		QList<QWidget *> openFiles;
		void setupEditors();
		void setupTextEditor(QTextEdit *editor);
		void setupTreeView();
};

#endif // MAINWINDOW_H
