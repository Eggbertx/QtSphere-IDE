#include <QProcess>
#include <QFileDialog>
#include <QMenu>
#include <QCloseEvent>
#include <QTime>
#include <QDir>
#include <QTextStream>
#include <QToolButton>
#include <QWidgetAction>
#include <QTreeView>
#include <QHeaderView>
#include <QComboBox>
#include <QKeySequence>
#include <QListWidget>
#include <QListWidgetItem>
#include <QShortcut>
#include <assert.h>

#include "mainwindow.h"
#include "util.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "projectpropertiesdialog.h"
#include "settingswindow.h"
#include "objects/spriteset.h"
#include "modifiedfilesdialog.h"
#include "qsiproject.h"

MainWindow* MainWindow::_instance = NULL;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
	Q_ASSERT(!_instance);
	_instance = this;
	ui->setupUi(this);
	ui->menuDebug->deleteLater();

	this->statusLabel = new QLabel("Ready.");
	ui->statusBar->addWidget(this->statusLabel);
	//ui->toolbarNewButton->setIcon(this->style()->standardIcon(QStyle::SP_FileIcon));
	ui->toolbarOpenButton->setIcon(this->style()->standardIcon(QStyle::SP_DialogOpenButton));
	ui->toolbarSaveButton->setIcon(this->style()->standardIcon(QStyle::SP_DriveFDIcon));

	ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

	ui->centralWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->centralWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(showContextMenu(const QPoint&)));

	QAction *nextTabAction = new QAction(this);
	nextTabAction->setShortcut(Qt::CTRL|Qt::Key_PageDown);
	connect(nextTabAction, SIGNAL(triggered()), this, SLOT(nextTab()));
	this->addAction(nextTabAction);

	QAction *prevTabAction = new QAction(this);
	prevTabAction->setShortcut(Qt::CTRL|Qt::Key_PageUp);
	connect(prevTabAction, SIGNAL(triggered()), this, SLOT(prevTab()));
	this->addAction(prevTabAction);

	QToolButton* toolButton = new QToolButton();
	toolButton->setIcon(this->style()->standardIcon(QStyle::SP_FileIcon));
	toolButton->setToolTip("New file");
	toolButton->setMenu(ui->menuNew);
	toolButton->setPopupMode(QToolButton::InstantPopup);
	QWidgetAction* toolButtonAction = new QWidgetAction(this);
	toolButtonAction->setDefaultWidget(toolButton);
	
	//ui->mainToolBar->insertAction(toolButtonAction);
	ui->mainToolBar->insertAction(ui->toolbarOpenButton,toolButtonAction);

	
	openProject("");

	QList<int> mainSplitterList;
	QList<int> consoleSplitterList;
	mainSplitterList << 150 << this->width()-150;
	consoleSplitterList << 350 << 1;
	ui->splitter->setSizes(mainSplitterList);
	ui->consoleSplitter->setSizes(consoleSplitterList);
}

void MainWindow::addWidgetTab(QWidget* widget, QString tabname) {
	widget->setObjectName(tabname + QString::number(ui->openFileTabs->count()));
	this->openFiles.append(widget);
	ui->openFileTabs->insertTab(ui->openFileTabs->count(), widget, tabname);
	ui->openFileTabs->setCurrentIndex(ui->openFileTabs->count()-1);
}

QString MainWindow::getStatus() {
	foreach(QObject* child, ui->statusBar->children()) {
		if(getWidgetType(child) == "QLabel") {
			QLabel* lbl = qobject_cast<QLabel*>(child);
			return lbl->text();
		}
	}
	return "";
}

void MainWindow::setStatus(QString status) {
	QLabel* msg = ui->statusBar->findChild<QLabel *>();
	if(msg == nullptr) return;
	msg->setText(status);
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
	event->accept(); // leaving this here until we can reliably check to see if an open file has been modified
	/*event->ignore();
	if(handleModifiedFiles() > 0)
		event->accept();*/
}

void MainWindow::console(QVariant s, int which) {
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
	if(ui->openFileTabs->count() == 0) return;
	QObjectList tabChildren = ui->openFileTabs->currentWidget()->children();
	QWidget* currentWidget = ui->openFileTabs->currentWidget();
	if(QString(currentWidget->metaObject()->className()) == "QTextEdit") {
		QTextEdit* currentEditor = static_cast<QTextEdit*>(ui->openFileTabs->currentWidget());

		QString saveFileName = QFileDialog::getSaveFileName(this,
								"Save script", "","Script (*.js);;Text file (*.txt);;All files (*)");
		QFile saveFile(saveFileName);
		if(!saveFile.open(QIODevice::WriteOnly)) {
			QString errorString = saveFile.errorString();
			if(errorString != "No file name specified")
				this->console("Failed saving file: " + saveFile.errorString());
			return;
		} else {
			QTextStream out(&saveFile);
			out.setCodec("UTF-8");
			out << currentEditor->document()->toPlainText();
			saveFile.flush();
			saveFile.close();
		}
	}
}

void MainWindow::showContextMenu(const QPoint &pos) {
	//QPoint globalPos = ui->treeWidget->mapToGlobal(pos);
	QPoint globalPos = ui->treeView->mapToGlobal(pos);

	QMenu rMenu;
	QWidget* currentWidget = ui->centralWidget->childAt(pos);
	if(currentWidget->objectName() == "treeWidget")
		rMenu.addAction("treeWidget");
	else if(currentWidget->objectName() == "treeView")
		rMenu.addAction("treeView");
	else
		rMenu.addAction(getWidgetType(currentWidget));

	rMenu.addAction("Menu item 1");
	rMenu.addAction(ui->toolbarPlayGame);

	QAction* selectedItem = rMenu.exec(globalPos);
	if (selectedItem) {
		this->console("selectedItem", 1);
	} else {
		this->console("notSelectedItem", 1);
	}
}

void MainWindow::setupTextEditor(QTextEdit *editor) {
	QFont font("Monospace", 10);
	font.setFixedPitch(true);
	editor->setFont(font);
}

void MainWindow::setupTreeWidget() {
	if(this->project->getPath() == "") return;
	QFileSystemModel *model = new QFileSystemModel();
	model->setRootPath(this->project->getPath());

	ui->treeView->setModel(model);
	for (int i = 1; i < model->columnCount(); ++i)
		ui->treeView->hideColumn(i);
	ui->treeView->setRootIndex(model->index(this->project->getPath()));
	this->setWindowTitle("QtSphereIDE " + QString(VERSION) + " - " + QDir(this->project->getPath()).dirName());
}

void MainWindow::openFile(QString fileName) {
	QString fn;
	if(fileName == "") {
		fn = QFileDialog::getOpenFileName(this,
			"Open file", QDir::currentPath(),
			"All supported files (*.sgm *.txt *.js *.mjs *.rmp *.rss *.rws);;"
			"Sphere projects (*.sgm);;"
			"Script files (*.js *.mjs);;"
			"Text files (*.txt);;"
			"All files (*.*)"
		);
	} else {
		fn = fileName;
	}

	if(fn == "") return;

	QFile* file = new QFile(fn);

	QString fileExtension = fn.right(fn.length() - fn.lastIndexOf("."));
	QWidget newTab;
	if (!file->open(QIODevice::ReadWrite | QIODevice::Text)) return;
	QFileInfo fi = QFileInfo(fn);

	if(fileExtension == ".rss") {
		Spriteset *ssWidget = new Spriteset(this);
		
		if(ssWidget->open(fn.toLatin1().data()))
			this->addWidgetTab(ssWidget,fi.fileName());
	} else {
		QByteArray bytes = file->readAll();
		QTextEdit* newTextEdit = new QTextEdit(this);
		newTextEdit->setText(bytes);
		this->setupTextEditor(newTextEdit);
		this->addWidgetTab(newTextEdit, fi.fileName());
	}
}

void MainWindow::openProject(QString fileName, bool reopen) {
	this->project = new QSIProject(fileName, this);	
	QString baseName = QDir(fileName).dirName();
	if(reopen) this->console("Reloading \"" + baseName + "\"");
	else this->console("Loading project: " + fileName);


	bool useSGM = QFileInfo(this->project->getPath() + "/game.sgm").exists();
	bool useSSProj = QFileInfo(this->project->getPath() + baseName + ".ssproj").exists();
	bool useJSON = QFileInfo(this->project->getPath() + "/game.json").exists();
	bool useCellscript = QFileInfo(this->project->getPath() + "/Cellscript.mjs").exists();

	if(!useSGM && !useSSProj && !useJSON && !useCellscript) {
		ui->actionRun_Cell->setEnabled(false);
		ui->actionPackage_Game->setEnabled(false);
		ui->actionProject_Properties->setEnabled(false);
	} else {
		ui->actionRun_Cell->setEnabled(true);
		ui->actionPackage_Game->setEnabled(true);
		ui->actionProject_Properties->setEnabled(true);
	}
	this->setupTreeWidget();
}

void MainWindow::readSGM(QString path) {
	if(!QFileInfo(this->project->getPath() + "/game.sgm").exists()) return;
	QFile* sgmFile = new QFile(this->project->getPath() + "/game.sgm");
	if(sgmFile->open(QIODevice::ReadOnly)) {
		QTextStream in(sgmFile);
		while(!in.atEnd()) {
			QStringList arr = in.readLine().split("=");
			if(arr.length() != 2) return;
			QString key = arr.first();
			QString value = arr.last();
			if(key == "name") this->project->name = value;
			else if(key == "author") this->project->author = value;
			else if(key == "description") this->project->summary = value;
			else if(key == "screen_width") this->project->width = value.toInt();
			else if(key == "screen_height") this->project->height = value.toInt();
			else if(key == "script") this->project->script = value;
		}
		sgmFile->close();
	}
}

void MainWindow::handleModifiedFiles() {
	if(this->openFiles.count() == 0) return;
	ModifiedFilesDialog mfd(this);
	int num_modified = 0;
	QList<QTextEdit *> openEditors = ui->openFileTabs->findChildren<QTextEdit *>();

	for(int t = 0; t < openEditors.count(); t++) {
		/* if(openEditors.at(t)->document()->toPlainText() != this->openFiles.at(t)->text) {
			mfd.addModifiedItem(this->openFiles.at(t));
			num_modified++;
		}*/
	}
	if(num_modified > 0) mfd.exec();
}

void MainWindow::on_actionExit_triggered() {
	handleModifiedFiles();
	this->close(); // just temporary until I have a way to test if the currently open file has been modified
	//if(handleModifiedFiles() > 0) this->close();
}

void MainWindow::on_actionConfigure_QtSphere_IDE_triggered() {
	SettingsWindow settingsWindow(this);
	settingsWindow.setModal(true);
	settingsWindow.exec();
}

void MainWindow::on_toolbarNewButton_triggered() {
	QTextEdit* newTextEdit = new QTextEdit(this);
	newTextEdit->setObjectName("textEdit" + QString::number(ui->openFileTabs->count()));
	this->setupTextEditor(newTextEdit);
	this->openFiles.append(newTextEdit);
	ui->openFileTabs->insertTab(0, newTextEdit, "<Untitled>");
	ui->openFileTabs->setCurrentIndex(0);
}

void MainWindow::on_toolbarSaveButton_triggered() {
	this->saveCurrentTab();
}

void MainWindow::on_toolbarOpenButton_triggered() {
	this->openFile();
}

void MainWindow::on_actionOpenFile_triggered() {
	this->openFile();
}

void MainWindow::on_openFileTabs_tabCloseRequested(int index) {
	this->openFiles.removeAt(index);
	ui->openFileTabs->removeTab(index);
}

void MainWindow::on_actionUndo_triggered() {
	ui->openFileTabs->currentWidget()->findChildren<QTextEdit *>().at(0)->undo();
}

void MainWindow::on_toolbarProjectProperties_triggered() {
	ProjectPropertiesDialog propertiesDialog;
	propertiesDialog.exec();
}

void MainWindow::on_newProject_triggered() {
	ProjectPropertiesDialog propertiesDialog(true);
	if(propertiesDialog.exec() == QDialog::Accepted) {
		QDir newDir = QDir(propertiesDialog.getProject()->getPath());
		if(newDir.exists() || !newDir.mkpath("")) {
			this->console("Error creating new project.");
			return;
		}
	}
}

void MainWindow::on_actionProject_Properties_triggered() {
	ProjectPropertiesDialog propertiesDialog;

	propertiesDialog.exec();
}

void MainWindow::nextTab() {
	int current = ui->openFileTabs->currentIndex();
	int numTabs = ui->openFileTabs->count();
	if(current == numTabs - 1) ui->openFileTabs->setCurrentIndex(0);
	else ui->openFileTabs->setCurrentIndex(current+1);
}

void MainWindow::prevTab() {
	int current = ui->openFileTabs->currentIndex();
	int numTabs = ui->openFileTabs->count();
	if(current == 0) ui->openFileTabs->setCurrentIndex(numTabs - 1);
	else ui->openFileTabs->setCurrentIndex(current-1);
}


void MainWindow::on_newPlainTextFile_triggered() {
	QTextEdit* newTextEdit = new QTextEdit(this);
	newTextEdit->setObjectName("textEdit" + QString::number(ui->openFileTabs->count()));
	this->setupTextEditor(newTextEdit);
	this->openFiles.append(newTextEdit);
	ui->openFileTabs->insertTab(0, newTextEdit, "<Untitled>");
	ui->openFileTabs->setCurrentIndex(0);
}

void MainWindow::on_newTaskButton_clicked() {
	int rowCount = ui->tableWidget->rowCount();
	ui->tableWidget->insertRow(rowCount);
	QTableWidgetItem* h = new QTableWidgetItem("");
	h->setCheckState(Qt::Unchecked);
	ui->tableWidget->setItem(rowCount-1, 1, h);
}

void MainWindow::on_actionOpenProject_triggered() {
	this->openProject(QFileDialog::getExistingDirectory(this,"Choose project path"));
}

void MainWindow::on_actionRefresh_triggered() {
	this->openProject(this->project->getPath(), true);
}
