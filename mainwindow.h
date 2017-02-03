#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSyntaxHighlighter>
#include <QTextEdit>
#include <QFileSystemModel>
#include <QVariant>
namespace Ui {
	class MainWindow;
	extern MainWindow w;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();
		QFileSystemModel *fileTree;

		void openFile(QString fileName);
		void openProject(QString fileName);

        // triggered whenever a file tab is clicked
        void prepareForText();
        void prepareForImage();
        void prepareForMap();
        void prepareForTileSet();
        void prepareForSpriteset();
		void console(QVariant s, int which = 1);
		void closeEvent(QCloseEvent* event);

	private slots:
		void on_actionMiniRT_API_triggered();

		void on_actionAbout_triggered();

		void on_actionSphere_2_0_API_triggered();

		void on_actionExit_triggered();

		void on_actionOpen_triggered();

		void showContextMenu(const QPoint& pos);

        void on_actionConfigure_QtSphere_IDE_triggered();

private:
		Ui::MainWindow *ui;
		void setupEditor();
		QTextEdit *editor;

};


#endif // MAINWINDOW_H
