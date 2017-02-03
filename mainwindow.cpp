#include <iostream>
#include <QProcess>
#include <QFileDialog>
#include <QTreeWidget>
#include <QMenu>
#include <QSplitter>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTime>

#include "mainwindow.h"
#include "util.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "settingswindow.h"
#include "highlighter.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	this->console("こんばんは！");

	ui->statusBar->showMessage("Ready");

	ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

	QTableWidgetItem* h = new QTableWidgetItem("h");
	h->setCheckState(Qt::Unchecked);

	ui->tableWidget->setItem(0, 1, h);


	ui->centralWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->centralWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(showContextMenu(const QPoint&)));

	prepareForText();
	setupEditor();
	openProject("");

	QList<int> mainSplitterList;
	QList<int> consoleSplitterList;
	mainSplitterList << 150 << this->width()-150;
	consoleSplitterList << 350 << 1;
	ui->splitter->setSizes(mainSplitterList);
	ui->consoleSplitter->setSizes(consoleSplitterList);
}

MainWindow::~MainWindow() {
	disconnect(ui->centralWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(showContextMenu(const QPoint&)));
	delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event) {
	return;
	event->ignore();
	if(handleModifiedFiles() > 0)
		event->accept();
}

void MainWindow::console(QVariant s, int which) {

	QString prefix = "[" + QTime::currentTime().toString("h:mm:s ap") + "] ";
	switch (which) {
		case 0:
            ui->buildLogText->insertPlainText(prefix + s.toString() + "\n");
		break;
		case 1:
            ui->consoleText->insertPlainText(prefix + s.toString() + "\n");
		break;
		case 2:
			//
		break;
		default:
            ui->consoleText->insertPlainText(prefix + s.toString() + "\n");
		break;
	}
}


void MainWindow::on_actionAbout_triggered() {
	AboutDialog aboutDialog;
	aboutDialog.setModal(true);
	aboutDialog.exec();

}

void MainWindow::on_actionSphere_2_0_API_triggered() {
	QProcess process;
}


void MainWindow::on_actionMiniRT_API_triggered() {
	QProcess process;
}

void MainWindow::setupEditor() {
	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(12);

	ui->toolbarNewButton->setIcon(this->style()->standardIcon(QStyle::SP_FileIcon));
	ui->toolbarOpenButton->setIcon(this->style()->standardIcon(QStyle::SP_DialogOpenButton));
    ui->toolbarSaveButton->setIcon(this->style()->standardIcon(QStyle::SP_DriveFDIcon));

	QObjectList widgets =  ui->openFileTabs->findChildren<QObject *>();

	foreach(QObject* obj, widgets) {
		if(obj->objectName().indexOf("textEdit") == 0 && obj->isWidgetType()) {
			QTextEdit* editor = qobject_cast<QTextEdit*>(obj);
			editor->setFont(font);
			Highlighter* highlighter = new Highlighter(editor->document());

			QFile file("main.js");
			if(file.open(QFile::ReadOnly | QFile::Text)) {
				editor->setPlainText("file.readAll()");
			}
		}
	}
}

void MainWindow::showContextMenu(const QPoint &pos) {
	QPoint globalPos = ui->treeWidget->mapToGlobal(pos);


	QMenu rMenu;
	QWidget* currentWidget = ui->centralWidget->childAt(pos);

	if(currentWidget->objectName() == "treeWidget")
		rMenu.addAction("treeWidget");
	else if(currentWidget->objectName() == "treeView")
		rMenu.addAction("treeView");
	else
		rMenu.addAction(currentWidget->objectName());

	rMenu.addAction("Menu item 1");
	rMenu.addAction(ui->toolbarPlayGame);

	QAction* selectedItem = rMenu.exec(globalPos);



	if (selectedItem) {
		ui->consoleText->insertPlainText("selectedItem");
	} else {
		ui->consoleText->insertPlainText("not selectedItem");
	}

}

void MainWindow::prepareForText() {
    QList<int> paneSizes; // size of horizontal pane and main area
    paneSizes << 200 << 300;
    ui->splitter->setSizes(paneSizes);
}

void MainWindow::openProject(QString fileName) {
    qDebug().noquote() << "Loading " << fileName << "...";
    this->console("Loading " + QVariant(fileName).toString());
}

void MainWindow::on_actionExit_triggered() {
	this->close();
	//if(handleModifiedFiles() > 0) this->close();
}


void MainWindow::on_actionOpen_triggered() {
	QFileDialog openDialog(this);
	openDialog.setFileMode(QFileDialog::ExistingFile);

	QStringList filters;
	filters << "Scripts (*.js *.ts)" << "All files (*)";

	openDialog.setNameFilters(filters);
	QStringList filenames;
	if(openDialog.exec()) {
		filenames = openDialog.selectedFiles();
	}
}

void MainWindow::on_actionConfigure_QtSphere_IDE_triggered() {
    SettingsWindow settingsWindow;
    settingsWindow.setModal(true);
    settingsWindow.exec();
}
