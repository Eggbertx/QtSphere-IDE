#include <QProcess>
#include <QFileDialog>
#include <QMenu>
#include <QCloseEvent>
#include <QTime>
#include <QDesktopServices>
#include <QDir>
#include <QMediaPlayer>
#include <QDirIterator>
#include <QTextStream>
#include <QToolButton>
#include <QWidgetAction>
#include <QTreeView>
#include <QKeySequence>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "util.h"
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

	this->statusLabel = new QLabel("Ready.");
	ui->statusBar->addWidget(this->statusLabel);
	ui->toolbarOpenButton->setIcon(this->style()->standardIcon(QStyle::SP_DialogOpenButton));
	ui->toolbarSaveButton->setIcon(this->style()->standardIcon(QStyle::SP_DriveFDIcon));
	ui->playToggleButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
	ui->stopButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaStop));
	ui->repeatButton->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
	ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

	ui->centralWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->centralWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(showContextMenu(const QPoint&)));

	/*
	 * move to the next file tab (or to the first if we're at the end)
	 * when the Page Down key is pressed
	 */
	QAction *nextTabAction = new QAction(this);
	nextTabAction->setShortcut(Qt::CTRL|Qt::Key_PageDown);
	connect(nextTabAction, SIGNAL(triggered()), this, SLOT(nextTab()));
	this->addAction(nextTabAction);

	/*
	 * Move to the previous file tab (or to the last if we're at the first)
	 * when the Page Up key is pressed
	 */
	QAction *prevTabAction = new QAction(this);
	prevTabAction->setShortcut(Qt::CTRL|Qt::Key_PageUp);
	connect(prevTabAction, SIGNAL(triggered()), this, SLOT(prevTab()));
	this->addAction(prevTabAction);

	// add new file drop-down "button" and copy the menu from the file menu
	QToolButton* toolButton = new QToolButton();
	toolButton->setIcon(this->style()->standardIcon(QStyle::SP_FileIcon));
	toolButton->setToolTip("New file");
	toolButton->setMenu(ui->menuNew);
	toolButton->setPopupMode(QToolButton::InstantPopup);
	QWidgetAction* toolButtonAction = new QWidgetAction(this);
	toolButtonAction->setDefaultWidget(toolButton);
	ui->mainToolBar->insertAction(ui->toolbarOpenButton,toolButtonAction);

	QList<int> mainSplitterList;
	QList<int> consoleSplitterList;
	mainSplitterList << 150 << this->width()-150;
	consoleSplitterList << 350 << 1;
	ui->splitter->setSizes(mainSplitterList);
	ui->consoleSplitter->setSizes(consoleSplitterList);
	this->project = new QSIProject("", this);
	this->mediaPlayer = new QMediaPlayer(this);
	connect(this->mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this,
			SLOT(audioStateChanged(QMediaPlayer::State)));
	connect(this->mediaPlayer, SIGNAL(positionChanged(qint64)), this,
			SLOT(audioPositionChanged(qint64)));
	connect(this->mediaPlayer, SIGNAL(durationChanged(qint64)), this,
			SLOT(audioDurationChanged(qint64)));
}

void MainWindow::addWidgetTab(QWidget* widget, QString tabname) {
	widget->setObjectName(tabname + QString::number(ui->openFileTabs->count()));
	this->openFiles.append(widget);
	ui->openFileTabs->insertTab(ui->openFileTabs->count(), widget, tabname);
	ui->openFileTabs->setCurrentIndex(ui->openFileTabs->count()-1);
}

QString MainWindow::getStatus() {
	return this->statusLabel->text();
}

void MainWindow::setStatus(QString status) {
	this->statusLabel->setText(status);
}

MainWindow::~MainWindow() {
	disconnect(ui->centralWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(showContextMenu(const QPoint&)));

	disconnect(SIGNAL(triggered()), this, SLOT(nextTab()));
	disconnect(SIGNAL(triggered()), this, SLOT(prevTab()));

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
			ui->consoleText->insertPlainText(line);
		break;
		case 1:
			ui->buildLogText->insertPlainText(line);
		break;
		default:
			ui->consoleText->insertPlainText(line);
		break;
	}
}

void MainWindow::on_actionAbout_triggered() {
	AboutDialog aboutDialog;
	aboutDialog.exec();
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
	QPoint globalPos = ui->treeView->mapToGlobal(pos);

	QMenu rMenu;
	QWidget* currentWidget = ui->centralWidget->childAt(pos);
	if(currentWidget->objectName() == "treeView")
		rMenu.addAction("treeView");
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

void MainWindow::setupTreeView() {
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
		QString usePath = QString();
		if(this->project && this->project->getPath() != "") {
			usePath = this->project->getPath();
		}
		fn = QFileDialog::getOpenFileName(this,
			"Open file", usePath,
			"All supported files (*.sgm *.txt *.js *.mjs *.rmp *.rss *.rws);;"
			"Sphere projects (*.sgm);;"
			"Script files (*.js *.mjs);;"
			"Text files (*.txt);;"
			"Audio files (*.wav *.ogg *.mp3 *.flac *.it *.mod *.s3m *.xm *.sid);;"
			"All files (*.*)"
		);
	} else {
		fn = fileName;
	}

	if(fn == "") return;

	QFile* file = new QFile(fn);

	QStringList audioList;

	audioList <<
				"wav" << "ogg" << "mp3" << "flac" << "it" <<
				"mod" << "s3m" << "xm" << "sid";

	if (!file->open(QIODevice::ReadWrite | QIODevice::Text)) return;
	QFileInfo fi = QFileInfo(fn);
	QString fileExtension = fi.suffix();
	if(fileExtension == "rss") {
		Spriteset *ssWidget = new Spriteset(this);
		if(ssWidget->open(fn.toLatin1().data()))
			this->addWidgetTab(ssWidget,fi.fileName());
	} else if(audioList.indexOf(fileExtension) > -1) {
		this->mediaPlayer->stop();
		this->mediaPlayer->setMedia(QUrl::fromLocalFile(fn));
		this->audioDuration = this->mediaPlayer->duration();
		ui->horizontalSlider->setEnabled(true);
		ui->horizontalSlider->setMinimum(0);
		ui->horizontalSlider->setMaximum(100);
		//ui->horizontalSlider->setRange(0,this->audioDuration);
		this->mediaPlayer->play();
		ui->currAudioLbl->setText(fi.fileName());
		ui->playToggleButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPause));
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

	bool useSSProj = false;
	bool useSGM = false;
	bool useJSON = false;
	bool useCellscript = false;
	QString projectFilePath = "";
	QDirIterator iterator(this->project->getPath());
	while(iterator.hasNext()) {
		iterator.next();
		if(QFileInfo(iterator.filePath()).isFile() &&
		   QFileInfo(iterator.filePath()).suffix() == "ssproj") {
			projectFilePath = iterator.filePath();
			useSSProj = true;
			break;
		}
	}
	if(!useSSProj) {
		useSGM = QFileInfo(this->project->getPath() + "/game.sgm").exists();
		useJSON = QFileInfo(this->project->getPath() + "/game.json").exists();
		useCellscript = QFileInfo(this->project->getPath() + "/Cellscript.mjs").exists();
	}

	if(!useSSProj && !useSGM && !useJSON && !useCellscript) {
		ui->actionProject_Properties->setEnabled(false);
	} else {
		ui->actionProject_Properties->setEnabled(true);
	}

	if(!useCellscript) {
		ui->actionRun_Cell->setEnabled(false);
		ui->actionPackage_Game->setEnabled(false);
	} else {
		ui->actionRun_Cell->setEnabled(true);
		ui->actionPackage_Game->setEnabled(true);
	}

	this->setupTreeView();
	if(useSSProj) this->readProjectFile("");
	this->readProjectFile(projectFilePath);
}

void MainWindow::readProjectFile(QString filename) {
	QString fileType = QFileInfo(filename).suffix().toLower();
	QFile* projectFile = new QFile(filename);

	Q_ASSERT(filename != "");
	Q_ASSERT(fileType != "");
	Q_ASSERT(projectFile != NULL);

	if(fileType == "ssproj" || fileType == "sgm") {
		if(projectFile->open(QIODevice::ReadOnly)) {
			QTextStream in(projectFile);
			while(!in.atEnd()) {
				QStringList arr = in.readLine().split("=");
				if(arr.length() != 2) return;
				QString key = arr.first();
				QString value = arr.last();
				if(key == "name") this->project->name = value;
				else if(key == "author") this->project->author = value;
				else if(key == "description") this->project->summary = value;
				else if(key == "screen_width" || key == "screenWidth") this->project->width = value.toInt();
				else if(key == "screen_height" || key == "screenHeight") this->project->height = value.toInt();
				else if(key == "script" || key == "mainScript") this->project->script = value;
				else if(key == "compiler") this->project->setCompiler(value);
				else if(key == "buildDir") this->project->buildDir = value;
			}
		}
	} else {

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
	ProjectPropertiesDialog propertiesDialog(false, this->project);

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

void MainWindow::on_actionQSIGithub_triggered() {
	QDesktopServices::openUrl(QUrl("https://github.com/Eggbertx/QtSphere-IDE"));
}

void MainWindow::on_actionMSGithub_triggered() {
	QDesktopServices::openUrl(QUrl("https://github.com/fatcerberus/miniSphere"));
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index) {
	QFileSystemModel* model = new QFileSystemModel();
	model->setRootPath(this->project->getPath());
	if(model->fileInfo(index).isFile()) this->openFile(model->filePath(index));
}

void MainWindow::on_playToggleButton_clicked() {
	switch(this->mediaPlayer->state()) {
		case QMediaPlayer::StoppedState: {
			this->mediaPlayer->setPosition(0);
			this->mediaPlayer->play();
			break;
		}
		case QMediaPlayer::PlayingState:
			this->mediaPlayer->pause();
			break;
		case QMediaPlayer::PausedState:
			this->mediaPlayer->play();
			break;
	}
}

void MainWindow::on_stopButton_clicked() {
	this->mediaPlayer->stop();
	ui->playToggleButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
}

void MainWindow::on_repeatButton_clicked(bool checked) {
	this->loopingAudio = checked;
}

void MainWindow::audioDurationChanged(qint64 duration) {
	this->audioDuration = duration;
}

void MainWindow::audioPositionChanged(qint64 position) {
	float floatPos = (float)position;
	int percent = floatPos/this->audioDuration*100;
	if(!ui->horizontalSlider->isSliderDown()) ui->horizontalSlider->setSliderPosition(percent);
}

void MainWindow::audioStateChanged(QMediaPlayer::State state) {
	switch(state) {
		case QMediaPlayer::PlayingState:
			ui->playToggleButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPause));
			break;
		case QMediaPlayer::PausedState:
			ui->playToggleButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
			break;
		case QMediaPlayer::StoppedState: {
			ui->playToggleButton->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));
			if(this->mediaPlayer->position() == this->audioDuration && this->loopingAudio) {
				this->mediaPlayer->play();
			}
			break;
		}
	}
}

void MainWindow::on_horizontalSlider_valueChanged(int value) {
	if(!ui->horizontalSlider->isSliderDown()) return;
	qint64 newPos = 0;
	float floatPos = (float)value;
	newPos = this->audioDuration*(floatPos/100);
	this->mediaPlayer->setPosition(newPos);
}
