#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QFileSystemModel>
#include <QVariant>

#include "objects/qsifile.h"
#include "texteffects.h"

#define MAINWINDOW_TITLE TARGET " (working title) " VERSION

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
		void openProject(QString fileName);
		void saveCurrentTab();

		void prepareForText();
		void prepareForCode();
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
		void on_actionOpen_triggered();
        void on_actionConfigure_QtSphere_IDE_triggered();
        void on_toolbarNewButton_triggered();
		void on_toolbarSaveButton_triggered();
        void on_toolbarOpenButton_triggered();
        void on_openFileTabs_tabCloseRequested(int index);
        void showContextMenu(const QPoint& pos);

private:
        static MainWindow* _instance;
		Ui::MainWindow *ui;
        QString status;
		TextEffects* highlighter;
        QVector<QSIFile *> openFiles;
		void setupEditor();
};

#endif // MAINWINDOW_H
