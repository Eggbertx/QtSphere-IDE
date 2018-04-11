#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QFileSystemModel>
#include <QWidget>
#include <QVariant>
#include <QComboBox>

#include "objects/spherefile.h"

#define MAINWINDOW_TITLE TARGET " " VERSION

namespace Ui {
	class MainWindow;
	extern MainWindow w;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = Q_NULLPTR);
		~MainWindow();
		QString getStatus();
		void setStatus(QString status);
		void openFile(QString fileName = "");
		void addWidgetTab(QWidget* widget, QString tabname);
		void openProject(QString fileName);
		void saveCurrentTab();
		void prepareForImage();
		void prepareForMap();
		void prepareForTileSet();
		void prepareForSpriteset();
		void handleModifiedFiles();
		void console(QVariant s, int which = 1);
		void closeEvent(QCloseEvent* event);
		static MainWindow* instance();

	private slots:
		void on_actionMiniRT_API_triggered();
		void on_actionAbout_triggered();
		void on_actionSphere_2_0_API_triggered();
		void on_actionExit_triggered();
		//void on_actionOpen_triggered();
		void on_actionConfigure_QtSphere_IDE_triggered();
		void on_toolbarNewButton_triggered();
		void on_toolbarSaveButton_triggered();
		void on_toolbarOpenButton_triggered();
		void on_openFileTabs_tabCloseRequested(int index);
		void showContextMenu(const QPoint& pos);
		void on_actionUndo_triggered();
		void on_toolbarProjectProperties_triggered();
        void on_newProject_triggered();
		void on_actionProject_Properties_triggered();
		void nextTab();
		void prevTab();


private:
		static MainWindow* _instance;
		Ui::MainWindow *ui;
		QString status;
		QList<QWidget *> openFiles;
		void setupEditors();
		void setupTextEditor(QTextEdit *editor);
};

#endif // MAINWINDOW_H
