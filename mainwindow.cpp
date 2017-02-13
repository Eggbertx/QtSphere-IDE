#include <QProcess>
#include <QFileDialog>
#include <QMenu>
#include <QCloseEvent>
#include <QTime>
#include <assert.h>
#include "mainwindow.h"
#include "util.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "settingswindow.h"
#include "highlighter.h"

MainWindow* MainWindow::_instance = NULL;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    Q_ASSERT(!_instance);
    _instance = this;

    ui->setupUi(this);
	ui->statusBar->showMessage("Ready");

	ui->toolbarNewButton->setIcon(this->style()->standardIcon(QStyle::SP_FileIcon));
	ui->toolbarOpenButton->setIcon(this->style()->standardIcon(QStyle::SP_DialogOpenButton));
	ui->toolbarSaveButton->setIcon(this->style()->standardIcon(QStyle::SP_DriveFDIcon));

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

MainWindow* MainWindow::instance() {
    if(!_instance)
        _instance = new MainWindow;

    return _instance;
}

void MainWindow::closeEvent(QCloseEvent* event) {
	return;
	event->ignore();
	if(handleModifiedFiles() > 0)
		event->accept();
}

void MainWindow::console(QVariant s, int which) {
    // thank you SO much to the folks at the Qt forums
    // for helping me get this nonsense to work

	QString prefix = "[" + QTime::currentTime().toString("h:mm:s ap") + "] ";
    QString line = prefix + s.toString();
	switch (which) {
		case 0:
            ui->buildLogText->insertPlainText(line);
		break;
		case 1:
            ui->consoleText->appendPlainText(line);
		break;
		case 2:
            // exceptions, probably make a QTableView
		break;
		default:
            ui->consoleText->insertPlainText(line);
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

void MainWindow::saveCurrentTab() {
	QObjectList tabChildren = ui->openFileTabs->currentWidget()->children();
    
    foreach (QObject* tab, tabChildren) {
        qDebug().noquote() << "Object: " << getWidgetType(tab);
    }
}

void MainWindow::setupEditor() {
	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(10);
	font.setStyleStrategy(QFont::PreferAntialias);
	int num_tabs = ui->openFileTabs->count();

	for(int i = 0; i < num_tabs; i++) {
		QObjectList children;
		for(int ch = 0; ch < ui->openFileTabs->widget(i)->children().count(); ch++) {
			if(getWidgetType(ui->openFileTabs->widget(i)->children()[ch]) == "QTextEdit") {
				QTextEdit* editor;
				editor = qobject_cast<QTextEdit*>(ui->openFileTabs->widget(i)->children()[ch]);
				editor->setFont(font);
				Highlighter* highlighter = new Highlighter(editor->document());
				children.append(ui->openFileTabs->widget(i)->children()[ch]);
				ui->openFileTabs->setCurrentIndex(0);
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

}

void MainWindow::openProject(QString fileName) {
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

void MainWindow::on_toolbarNewButton_triggered() {
	setTheme("ayy lmao");
	QTextEdit* newTextEdit = new QTextEdit("bleh.readAll()", ui->openFileTabs);
	newTextEdit->setObjectName("textEdit" + QString::number(ui->openFileTabs->count()) );
	Highlighter* highlighter = new Highlighter(newTextEdit->document());
	ui->openFileTabs->insertTab(0, newTextEdit, "<Untitled>");
	this->setupEditor();
}

void MainWindow::on_toolbarSaveButton_triggered() {
	this->saveCurrentTab();
}
