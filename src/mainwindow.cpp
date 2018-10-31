#include <QApplication>
#include <QProcess>
#include <QColorDialog>
#include <QFileDialog>
#include <QMenu>
#include <QCloseEvent>
#include <QTime>
#include <QDesktopServices>
#include <QDir>
#include <QLayout>
#include <QDirIterator>
#include <QTextStream>
#include <QToolButton>
#include <QWidgetAction>
#include <QStandardItemModel>
#include <QTreeView>
#include <QKeySequence>
#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "util.h"
#include "aboutdialog.h"
#include "importoptionsdialog.h"
#include "projectpropertiesdialog.h"
#include "settingswindow.h"
#include "formats/mapfile.h"
#include "formats/spriteset.h"
#include "modifiedfilesdialog.h"
#include "qsiproject.h"
#include "soundplayer.h"
#include "editors/mapeditor.h"
#include "editors/sphereeditor.h"
#include "editors/spriteseteditor.h"
#include "editors/texteditor.h"
#include "startpage.h"

MainWindow* MainWindow::_instance = nullptr;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
	Q_ASSERT(!_instance);
	_instance = this;
	ui->setupUi(this);
	ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

	this->statusLabel = new QLabel("Ready.");
	ui->statusBar->addWidget(this->statusLabel);
	ui->toolbarOpenButton->setIcon(this->style()->standardIcon(QStyle::SP_DialogOpenButton));
	ui->toolbarSaveButton->setIcon(this->style()->standardIcon(QStyle::SP_DriveFDIcon));
	ui->centralWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->taskListTable->setContextMenuPolicy(Qt::CustomContextMenu);

	/*
	 * move to the next file tab (or to the first if we're at the end)
	 * when the Page Down key is pressed
	 */
	QAction *nextTabAction = new QAction(this);
	nextTabAction->setShortcut(Qt::CTRL|Qt::Key_PageDown);
	connect(nextTabAction, SIGNAL(triggered()), this, SLOT(nextTab()));
	connect(ui->menuFile, SIGNAL(aboutToShow()), this, SLOT(checkCloseProjectOption()));
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
	mainSplitterList << 150 << this->width()-150;
	ui->splitter->setSizes(mainSplitterList);
	this->project = new QSIProject("", this);

	this->soundPlayer = new SoundPlayer();
	ui->mediaPlayerTab->layout()->addWidget(this->soundPlayer);
	ui->openFileTabs->addTab(new StartPage(ui->openFileTabs), "Start Page");
	this->updateTreeView();
	this->refreshRecentFiles();
}

MainWindow::~MainWindow() {
	disconnect(this, SLOT(nextTab()));
	disconnect(this, SLOT(prevTab()));
	disconnect(ui->menuFile, SIGNAL(aboutToShow()), this, SLOT(checkCloseProjectOption()));
	delete ui;
	delete this->soundPlayer;
	delete this->project;
}

MainWindow* MainWindow::instance() {
	if(!_instance)
		_instance = new MainWindow;
	return _instance;
}

/*void MainWindow::addWidgetTab(QWidget* widget, QString tabname) {
	widget->setObjectName(tabname + QString::number(ui->openFileTabs->count()));
	this->openEditors.append(widget);
	ui->openFileTabs->insertTab(ui->openFileTabs->count(), widget, tabname);
	ui->openFileTabs->setCurrentIndex(ui->openFileTabs->count()-1);
}*/

QString MainWindow::getStatus() {
	return this->statusLabel->text();
}

void MainWindow::setStatus(QString status) {
	this->statusLabel->setText(status);
}

QString MainWindow::getTheme() {
	return this->theme;
}

void MainWindow::setTheme(QString theme) {
	QString stylesheet = "";
	if(theme != "") {
		QFile* styleFile = new QFile(theme);
		if(!styleFile->open(QFile::ReadOnly)) {
			errorBox("Failed to open stylesheet (" + theme + "):" + styleFile->errorString());
			styleFile->close();
			delete styleFile;
			return;
		}
		stylesheet = QLatin1String(styleFile->readAll());
		styleFile->close();
		delete styleFile;
	}
	qApp->setStyleSheet(stylesheet);
	this->theme = theme;
}

void MainWindow::closeEvent(QCloseEvent* event) {
	event->accept(); // leaving this here until we can reliably check to see if an open file has been modified
	/*event->ignore();
	if(handleModifiedFiles() > 0)
		event->accept();*/
}

SphereEditor* MainWindow::getCurrentEditor() {
	int currentTabIndex = ui->openFileTabs->currentIndex();
	foreach (SphereEditor* editor, this->openEditors) {
		if(editor->tabIndex == currentTabIndex)
			return editor;
	}
	return nullptr;
}

void MainWindow::on_actionAbout_triggered() {
	AboutDialog aboutDialog;
	aboutDialog.exec();
}

void MainWindow::saveCurrentTab() {
	if(ui->openFileTabs->count() == 0) return;
	QObjectList tabChildren = ui->openFileTabs->currentWidget()->children();
	QWidget* currentWidget = ui->openFileTabs->currentWidget();
	QString widgetType = currentWidget->metaObject()->className();

	if(widgetType == "QTextEdit") {
		QTextEdit* currentEditor = dynamic_cast<QTextEdit*>(ui->openFileTabs->currentWidget());

		QString saveFileName = QFileDialog::getSaveFileName(this,
								"Save script", "","Script (*.js);;Text file (*.txt);;All files (*)");
		QFile saveFile(saveFileName);
		QFileInfo fi(saveFile);
		if(!saveFile.open(QIODevice::WriteOnly)) {
			QString errorString = saveFile.errorString();
			if(errorString != "No file name specified")
				errorBox("Failed saving file: " + saveFile.errorString());
			return;
		}
		QTextStream out(&saveFile);
		out.setCodec("UTF-8");
		out << currentEditor->document()->toPlainText();
		ui->openFileTabs->setTabText(ui->openFileTabs->currentIndex(), fi.fileName());
		ui->openFileTabs->setTabToolTip(ui->openFileTabs->currentIndex(), fi.filePath());
		saveFile.flush();
		saveFile.close();
	} else if(widgetType == "SpritesetEditor") {
		SpritesetEditor* currentEditor = dynamic_cast<SpritesetEditor*>(ui->openFileTabs->currentWidget());
		QString saveFileName = QFileDialog::getSaveFileName(this,
								"Save spriteset", "","Spriteset (*.rss);;All files (*)");
		if(!currentEditor->spriteset->save(saveFileName)) {
			errorBox("Failed saving file: " + currentEditor->spriteset->fileName());
		}
	}
}

void MainWindow::updateTreeView() {
	if(!this->projectLoaded || this->project->getPath(false) == "") {
		QStandardItemModel* blankTreeModel = new QStandardItemModel(0,0,ui->treeView);
		QStandardItem* child = new QStandardItem("<No open project>");
		blankTreeModel->appendRow(child);
		ui->treeView->setModel(blankTreeModel);
		return;
	}
	QFileSystemModel *model = new QFileSystemModel();
	model->setRootPath(this->project->getPath(false));
	ui->treeView->setModel(model);
	for (int i = 1; i < model->columnCount(); ++i)
		ui->treeView->hideColumn(i);
	ui->treeView->setRootIndex(model->index(this->project->getPath(false)));
	this->setWindowTitle("QtSphereIDE " + QString(VERSION) + " - " + QDir(this->project->getPath(false)).dirName());
}

void MainWindow::refreshProjectList() {

}

void MainWindow::openFile(QString filename) {
	QString fn;

	if(filename == "") {
		QString usePath = QString();
		if(this->project && this->project->getPath(false) != "") {
			usePath = this->project->getPath(false);
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
		fn = filename;
	}

	if(fn == "") return;

	QFile* file = new QFile(fn);

	QStringList audioList;
	audioList << "wav" << "ogg" << "mp3" << "flac" << "it" <<
				 "mod" << "s3m" << "xm" << "sid";

	if (!file->open(QIODevice::ReadWrite | QIODevice::Text)) {
		errorBox("Error opening" + fn + ": " + file->errorString());
		return;
	}
	QFileInfo fi = QFileInfo(fn);
	QString fileExtension = fi.suffix();
	if(fileExtension == "rmp") {
		MapEditor* rmpEditor = new MapEditor(this);
		if(rmpEditor->openFile(fi.filePath())) {
			rmpEditor->tabIndex = ui->openFileTabs->insertTab(0,rmpEditor, fi.fileName());
			ui->openFileTabs->setTabToolTip(0, file->fileName());
			ui->openFileTabs->setCurrentIndex(0);
			this->openEditors.append(rmpEditor);
		} else {
			return;
		}
	} else if(fileExtension == "rss") {
		SpritesetEditor* ssView = new SpritesetEditor(this);
		if(ssView->openFile(fi.filePath())) {
			ssView->tabIndex = ui->openFileTabs->insertTab(0, ssView, fi.fileName());
			ui->openFileTabs->setTabToolTip(0, file->fileName());
			ui->openFileTabs->setCurrentIndex(0);
			this->openEditors.append(ssView);
		}else {
			errorBox("Failed loading spriteset: " + fn);
			return;
		}
	} else if(audioList.indexOf(fileExtension) > -1) {
		this->soundPlayer->load(fn);
	} else {
		QByteArray bytes = file->readAll();
		TextEditor* newTextEdit = new TextEditor(this);
		newTextEdit->textEditorWidget->setText(bytes);
		newTextEdit->tabIndex = ui->openFileTabs->insertTab(0, newTextEdit->textEditorWidget, fi.fileName());

		ui->openFileTabs->setTabToolTip(0, file->fileName());
		ui->openFileTabs->setCurrentIndex(0);
		this->openEditors.append(newTextEdit);
	}
}

void MainWindow::openProject(QString filename) {
	this->project = new QSIProject(filename, this);
	this->projectLoaded = true;
	ui->menuProject->setEnabled(true);
	this->updateTreeView();
}

void MainWindow::handleModifiedFiles() {
	if(this->openEditors.count() == 0) return;
	ModifiedFilesDialog mfd(this);
	int num_modified = 0;
	QList<QTextEdit *> openEditors = ui->openFileTabs->findChildren<QTextEdit *>();

	for(int t = 0; t < openEditors.count(); t++) {
		/* if(openEditors.at(t)->document()->toPlainText() != this->openEditors.at(t)->text) {
			mfd.addModifiedItem(this->openEditors.at(t));
			num_modified++;
		}*/
	}
	if(num_modified > 0) mfd.exec();
}

void MainWindow::refreshRecentFiles() {
	QList<QAction*> recentItems = ui->menuOpen_Recent->actions();
	for(int a = 0; a < recentItems.length(); a++) {
		if(recentItems.at(a)->text() != "Clear recent") {
			delete recentItems.at(a);
		}

	}
	ui->menuOpen_Recent->addSeparator();

	QSettings settings;
	int size = settings.beginReadArray("recentProjects");
	for(int i = 0; i < size; ++i) {
		settings.setArrayIndex(i);
		ui->menuOpen_Recent->addAction(settings.value("project").toString());
	}
	settings.endArray();
	ui->menuOpen_Recent->addSeparator();

	size = settings.beginReadArray("recentFiles");
	for(int i = 0; i < size; i++) {
		settings.setArrayIndex(i);
		QAction* fileAction = ui->menuOpen_Recent->addAction(settings.value("file").toString());
		connect(fileAction, &QAction::triggered,
				this, [this,fileAction]() {
					this->openFile(fileAction->text());
				}
		);
	}
	settings.endArray();
}

void MainWindow::on_actionExit_triggered() {
	this->handleModifiedFiles();
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
	this->openEditors.removeAt(index);
	ui->openFileTabs->removeTab(index);
}

void MainWindow::on_actionUndo_triggered() {
	SphereEditor* currentEditor = this->getCurrentEditor();
	if(currentEditor == nullptr) return;

	switch (currentEditor->editorType()) {
	case SphereEditor::TextEditor: {
		TextEditor* editor = dynamic_cast<TextEditor*>(currentEditor);
		editor->undo();
		break;
	}
	default:
		break;
	}
	//ui->openFileTabs->currentWidget()->findChildren<QTextEdit *>().at(0)->undo();
}

void MainWindow::on_actionRedo_triggered() {
	SphereEditor* currentEditor = this->getCurrentEditor();
	if(currentEditor != nullptr) {
		qDebug() << "currentEditor type: " << currentEditor->editorType();
		currentEditor->redo();
	}
}

void MainWindow::on_toolbarProjectProperties_triggered() {
	ProjectPropertiesDialog propertiesDialog;
	propertiesDialog.exec();
}

void MainWindow::on_newProject_triggered() {
	ProjectPropertiesDialog propertiesDialog(true);
	if(propertiesDialog.exec() == QDialog::Accepted) {
		QDir newDir = QDir(propertiesDialog.getProject()->getPath(false));
		if(newDir.exists() || !newDir.mkpath("")) {
			errorBox("Error creating new project.");
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
	//QTextEdit* newTextEdit = new QTextEdit(this);
	TextEditor* newTextEdit = new TextEditor(this);
	newTextEdit->setObjectName("textEdit" + QString::number(ui->openFileTabs->count()));
	newTextEdit->tabIndex = ui->openFileTabs->insertTab(0, newTextEdit->textEditorWidget, "<Untitled>");
	ui->openFileTabs->setTabToolTip(0, "<Untitled>");
	this->openEditors.append(newTextEdit);
	ui->openFileTabs->setCurrentIndex(0);
}

void MainWindow::on_newTaskButton_clicked() {
	int rowCount = ui->taskListTable->rowCount();
	ui->taskListTable->insertRow(rowCount);
	QTableWidgetItem* h = new QTableWidgetItem("");
	h->setCheckState(Qt::Unchecked);
	ui->taskListTable->setItem(rowCount-1, 1, h);
	ui->delTaskButton->setEnabled(true);
}

void MainWindow::on_delTaskButton_clicked() {
	int rowCount = ui->taskListTable->rowCount();
	QList<QTableWidgetItem*> selected = ui->taskListTable->selectedItems();
	if(selected.length() == 0) ui->taskListTable->removeRow(rowCount - 1);
	else {
		for(int r = 0; r < selected.length(); r++) {
			ui->taskListTable->removeRow(selected.at(r)->row());
		}
	}
	if(ui->taskListTable->rowCount() == 0) ui->delTaskButton->setEnabled(false);
}


void MainWindow::on_actionOpenProject_triggered() {
	this->openProject(QFileDialog::getExistingDirectory(this,"Choose project path"));
}

void MainWindow::on_actionRefresh_triggered() {
	this->openProject(this->project->getPath(false));
}

void MainWindow::on_actionQSIGithub_triggered() {
	QDesktopServices::openUrl(QUrl("https://github.com/Eggbertx/QtSphere-IDE"));
}

void MainWindow::on_actionMSGithub_triggered() {
	QDesktopServices::openUrl(QUrl("https://github.com/fatcerberus/miniSphere"));
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index) {
	if(!this->projectLoaded) return;
	QFileSystemModel* model = dynamic_cast<QFileSystemModel*>(ui->treeView->model()); // new QFileSystemModel();
	model->setRootPath(this->project->getPath(false));
	if(model->fileInfo(index).isFile())
		this->openFile(model->filePath(index));
}

void MainWindow::checkCloseProjectOption() {
	if(!this->project || this->project->getPath(false) == "") ui->actionClose_Project->setEnabled(false);
	else ui->actionClose_Project->setEnabled(true);
}

void MainWindow::on_actionImage_to_Spriteset_triggered() {
	QString imagePath = QFileDialog::getOpenFileName(this,
		"Import image", "",
		"Image files (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm);;"
		"All files (*.*)"
	);
	if(imagePath == "") return; // user pressed cancel

	ImportOptionsDialog* dialog = new ImportOptionsDialog(this);
	if(dialog->exec() == QDialog::Accepted) {
		Spriteset* imported = Spriteset::fromImage(
			imagePath,dialog->getFrameSize(),
			dialog->removeDuplicatesChecked(),
			dialog->getTransparencyIn(),
			dialog->getTransparencyOut()
		);
		if(!imported->valid) return;

		SpritesetEditor* ssView = new SpritesetEditor(this);
		ssView->attach(imported);
		QFileInfo fi = QFileInfo(imagePath);
		ssView->tabIndex = ui->openFileTabs->insertTab(0, ssView, fi.fileName());
		ui->openFileTabs->setCurrentIndex(0);
	}
}

void MainWindow::on_actionSave_triggered() {
	this->saveCurrentTab();
}

void MainWindow::on_actionStart_Page_triggered() {
	int numTabs = ui->openFileTabs->count();
	for(int t = 0; t < numTabs; t++) {
		if(QString(ui->openFileTabs->widget(t)->metaObject()->className()) == "StartPage") return;
	}
	ui->openFileTabs->addTab(new StartPage(ui->openFileTabs), "Start Page");
}

void MainWindow::on_taskListTable_customContextMenuRequested(const QPoint &pos) {
	QMenu* contextMenu = new QMenu();

	contextMenu->addAction("Load task list...");
	contextMenu->addAction("Save task list");
	contextMenu->addAction("Save task list as...");
	QWidget* child = ui->taskListTable->childAt(pos);
	if(child != nullptr) {
		QAction* result = contextMenu->exec(child->mapToGlobal(pos));
		if(result == nullptr) return;
		QString text = result->text();
		if(text == "Load task list...") {

		} else if(text == "Save task list") {

		} else if(text == "Save task list as...") {

		}
	}
}

void MainWindow::on_actionSpherical_community_triggered() {
	QDesktopServices::openUrl(QUrl("http://www.spheredev.org/"));
}

void MainWindow::on_actionClearRecent_triggered() {
	QSettings settings;
	settings.remove("recentProjects");
	settings.remove("recentFiles");
	this->refreshRecentFiles();
}

void MainWindow::on_actionClose_Project_triggered() {
	this->project = new QSIProject("", this);
	this->projectLoaded = false;
	ui->menuProject->setEnabled(false);
	this->updateTreeView();
}
