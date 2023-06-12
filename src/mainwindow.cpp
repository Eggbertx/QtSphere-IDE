#include <QCloseEvent>
#include <QDesktopServices>
#include <QFileDialog>
#include <QSettings>
#include <QStandardItemModel>
#include <QWidgetAction>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qsiproject.h"
#include "util.h"
#include "dialogs/importoptionsdialog.h"
#include "dialogs/modifiedfilesdialog.h"
#include "dialogs/projectpropertiesdialog.h"
#include "dialogs/settingswindow.h"
#include "formats/mapfile.h"
#include "formats/spherefont.h"
#include "formats/spriteset.h"
#include "widgets/soundplayer.h"
#include "widgets/map/mapeditor.h"
#include "widgets/sphereeditor.h"
#include "widgets/spriteset/spriteseteditor.h"
#include "widgets/texteditor.h"
#include "widgets/startpage.h"

MainWindow* MainWindow::_instance = nullptr;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
	Q_ASSERT(!_instance);
	_instance = this;
	ui->setupUi(this);
	setMenuBar(ui->menuBar);
	ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_statusLabel = new QLabel("Ready.");
	ui->statusBar->addWidget(m_statusLabel);
	ui->toolbarOpenButton->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	ui->toolbarSaveButton->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
	ui->centralWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->taskListTable->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->openTaskListButton->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	ui->saveTaskListButton->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));

	/*
	 * move to the next file tab (or to the first if we're at the end)
	 * when the Page Down key is pressed
	 */
	QAction *nextTabAction = new QAction(this);
	nextTabAction->setShortcut(Qt::CTRL|Qt::Key_PageDown);
	connect(nextTabAction, SIGNAL(triggered()), this, SLOT(nextTab()));
	connect(ui->menuFile, SIGNAL(aboutToShow()), this, SLOT(checkCloseProjectOption()));
	addAction(nextTabAction);

	/*
	 * Move to the previous file tab (or to the last if we're at the first)
	 * when the Page Up key is pressed
	 */
	QAction *prevTabAction = new QAction(this);
	prevTabAction->setShortcut(Qt::CTRL|Qt::Key_PageUp);
	connect(prevTabAction, SIGNAL(triggered()), this, SLOT(prevTab()));
	addAction(prevTabAction);

	// add new file drop-down "button" and copy the menu from the file menu
	QToolButton* toolButton = new QToolButton();
	toolButton->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
	toolButton->setToolTip("New file");
	toolButton->setMenu(ui->menuNew);
	toolButton->setPopupMode(QToolButton::InstantPopup);
	QWidgetAction* toolButtonAction = new QWidgetAction(this);
	toolButtonAction->setDefaultWidget(toolButton);
	ui->mainToolBar->insertAction(ui->toolbarOpenButton,toolButtonAction);

	ui->splitter->setStretchFactor(1,4);
	ui->splitter->setSizes(QList<int>({200, width()-200}));

	m_newMapDialog = new NewMapDialog(m_project, this);
	m_project = new QSIProject(this);

	m_soundPlayer = new SoundPlayer();
	ui->soundPlayerTab->layout()->addWidget(m_soundPlayer);
	m_startPage = new StartPage(ui->openFileTabs);
	connect(m_startPage, SIGNAL(projectLoaded(QSIProject*)), this, SLOT(onProjectLoaded(QSIProject*)));
	ui->openFileTabs->addTab(m_startPage, "Start Page");

	m_engineSelector = new QComboBox(ui->mainToolBar);
	m_engineSelector->addItem("miniSphere");
	m_engineSelector->addItem("Sphere 1.x");
	m_engineSelectorAction = new QWidgetAction(ui->mainToolBar);
	m_engineSelectorAction->setDefaultWidget(m_engineSelector);
	ui->mainToolBar->addAction(m_engineSelectorAction);
	connect(m_engineSelector, SIGNAL(activated(int)), this, SLOT(onEngineDropdownChanged(int)));

	m_taskList = new QSITaskList(ui->taskListTable);
	QSettings settings;
	updateTreeView();
	refreshRecentFiles();
	setEngine(settings.value("whichEngine", "minisphere").toString());
}

MainWindow::~MainWindow() {
	disconnect(this, SLOT(nextTab()));
	disconnect(this, SLOT(prevTab()));
	disconnect(ui->menuFile, SIGNAL(aboutToShow()), this, SLOT(checkCloseProjectOption()));
	disconnect(m_startPage, SIGNAL(projectLoaded(QSIProject*)), this, SLOT(onProjectLoaded(QSIProject*)));
	disconnect(m_engineSelector, SIGNAL(activated(int)), this, SLOT(onEngineDropdownChanged(int)));
	delete ui;
	delete m_engineSelector;
	delete m_engineSelectorAction;
	delete m_newMapDialog;
	delete m_project;
	delete m_soundPlayer;
	delete m_startPage;
	QSettings settings;
	settings.setValue("geometry", geometry());
	settings.setValue("maximized", isMaximized());
}

MainWindow* MainWindow::instance() {
	if(!_instance)
		_instance = new MainWindow;
	return _instance;
}

int MainWindow::openProjectProperties(bool newFile, QSIProject* project, QWidget* parent) {
	ProjectPropertiesDialog propertiesDialog(newFile, project, parent);
	return propertiesDialog.exec();
}

QString MainWindow::getStatus() {
	return m_statusLabel->text();
}

void MainWindow::setStatus(QString status) {
	m_statusLabel->setText(status);
}

QString MainWindow::getTheme() {
	return m_theme;
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
	m_theme = theme;
}

void MainWindow::closeEvent(QCloseEvent* event) {
	event->accept(); // leaving this here until we can reliably check to see if an open file has been modified
	/*event->ignore();
	if(handleModifiedFiles() > 0)
		event->accept();*/
}

SphereEditor* MainWindow::getCurrentEditor() {
	int currentTabIndex = ui->openFileTabs->currentIndex();
	foreach(SphereEditor* editor, m_openEditors) {
		if(editor->getTabIndex() == currentTabIndex)
			return editor;
	}
	return nullptr;
}

void MainWindow::setEngine(QString which) {
	if(which == "legacy") {
		m_engineSelector->setCurrentIndex(1);
		ui->actionConfigure_Engine->setEnabled(true);
		ui->actionLegacyConfig->setEnabled(true);
		ui->toolbarPlayGame->setIcon(QIcon(":/icons/legacyengine-24x24.png"));
	} else {
		m_engineSelector->setCurrentIndex(0);
		ui->actionConfigure_Engine->setEnabled(false);
		ui->actionLegacyConfig->setEnabled(false);
		ui->toolbarPlayGame->setIcon(QIcon(":/icons/sphere-icon.png"));
	}
}

void MainWindow::on_actionAbout_Qt_triggered() {
	QMessageBox::aboutQt(this);
}

void MainWindow::on_actionAbout_triggered() {
	QMessageBox::about(this, "About QtSphere IDE",
		"QtSphere IDE v" VERSION "<br />"
		"Copyright 2019 by <a href=\"https://github.com/eggbertx\">Eggbertx</a><br /><br />"
		"See <a href=\"https://github.com/Eggbertx/QtSphere-IDE/blob/master/LICENSE.txt\">LICENSE.txt</a> for more information."
	);
}

void MainWindow::saveCurrentTab() {
	if(ui->openFileTabs->count() == 0) return;
	QObjectList tabChildren = ui->openFileTabs->currentWidget()->children();
	QWidget* currentWidget = ui->openFileTabs->currentWidget();
	QString widgetType = currentWidget->metaObject()->className();

	if(widgetType == "QTextEdit") {
		QTextEdit* currentEditor = dynamic_cast<QTextEdit*>(ui->openFileTabs->currentWidget());

		QString saveFileName = QFileDialog::getSaveFileName(this,
							"Save script", "","Script (*.mjs *.js);;Text file (*.txt);;All files (*)");
		QFile saveFile(saveFileName);
		QFileInfo fi(saveFile);
		if(!saveFile.open(QIODevice::WriteOnly)) {
			QString errorString = saveFile.errorString();
			if(errorString != "No file name specified")
				errorBox("Failed saving file: " + saveFile.errorString());
			return;
		}
		QTextStream out(&saveFile);
        // out.setCodec("UTF-8");
		out << currentEditor->document()->toPlainText();
		ui->openFileTabs->setTabText(ui->openFileTabs->currentIndex(), fi.fileName());
		ui->openFileTabs->setTabToolTip(ui->openFileTabs->currentIndex(), fi.filePath());
		saveFile.flush();
		saveFile.close();
	} else if(widgetType == "SpritesetEditor") {
		SpritesetEditor* currentEditor = dynamic_cast<SpritesetEditor*>(ui->openFileTabs->currentWidget());
		QString saveFileName = QFileDialog::getSaveFileName(this,
			"Save spriteset", "","Spriteset (*.rss);;All files (*)");
		Spriteset* editorSpriteset = currentEditor->getSpriteset();
		if(!editorSpriteset->save(saveFileName)) {
			errorBox("Failed saving file: " + editorSpriteset->fileName());
		}
	}
}

void MainWindow::updateTreeView() {
	if(!m_projectLoaded || m_project->getPath(false) == "") {
		QStandardItemModel* blankTreeModel = new QStandardItemModel(0,0,ui->treeView);
		QStandardItem* child = new QStandardItem("<No open project>");
		blankTreeModel->appendRow(child);
		ui->treeView->setModel(blankTreeModel);
		return;
	}
	QFileSystemModel *model = new QFileSystemModel();
	model->setRootPath(m_project->getPath(false));
	ui->treeView->setModel(model);
	for (int i = 1; i < model->columnCount(); ++i)
		ui->treeView->hideColumn(i);
	ui->treeView->setRootIndex(model->index(m_project->getPath(false)));
	setWindowTitle("QtSphereIDE " + QString(VERSION) + " - " + QDir(m_project->getPath(false)).dirName());
}

void MainWindow::openFile(QString filename) {
	QString fn;

	if(filename == "") {
		QString usePath = QString();
		if(m_project && m_project->getPath(false) != "") {
			usePath = m_project->getPath(false);
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
	if(fileExtension == "rfn") {

	} else if(fileExtension == "rmp") {
		MapEditor* rmpEditor = new MapEditor(this);
		if(rmpEditor->openFile(fi.filePath())) {
			rmpEditor->setTabIndex(ui->openFileTabs->insertTab(0,rmpEditor, fi.fileName()));
			ui->openFileTabs->setTabToolTip(0, file->fileName());
			ui->openFileTabs->setCurrentIndex(0);
			m_openEditors.append(rmpEditor);
		} else {
			return;
		}
	} else if(fileExtension == "rss") {
		SpritesetEditor* ssView = new SpritesetEditor(this);
		if(ssView->openFile(fi.filePath())) {
			ssView->setTabIndex(ui->openFileTabs->insertTab(0, ssView, fi.fileName()));
			ui->openFileTabs->setTabToolTip(0, file->fileName());
			ui->openFileTabs->setCurrentIndex(0);
			m_openEditors.append(ssView);
		}else {
			errorBox("Failed loading spriteset: " + fn);
			return;
		}
	} else if(fileExtension == "rts") {
		m_newMapDialog->setTilesetPath(fn);
		m_newMapDialog->open();
	} else if(audioList.indexOf(fileExtension) > -1) {
		m_soundPlayer->load(fn);
	} else {
		QByteArray bytes = file->readAll();
		TextEditor* newTextEdit = new TextEditor(this);
		newTextEdit->setText(bytes);
		newTextEdit->setTabIndex(ui->openFileTabs->insertTab(0, static_cast<QTextEdit*>(newTextEdit), fi.fileName()));
		ui->openFileTabs->setTabToolTip(0, file->fileName());
		ui->openFileTabs->setCurrentIndex(0);
		m_openEditors.append(newTextEdit);
	}
}

void MainWindow::openProject(QString filename) {
	m_projectLoaded = m_project->open(filename);
	if(m_projectLoaded) {
		ui->menuProject->setEnabled(true);
		ui->toolbarPlayGame->setEnabled(true);
		ui->toolbarProjectProperties->setEnabled(true);
		ui->actionRun_Cell->setEnabled(m_project->getCompiler() == "Cell");
		updateTreeView();
	} else {
		// qDebug() << filename << "not loaded.";
	}

	//ui->actionRun_Cell->setEnabled(false);
}

void MainWindow::setCurrentProject(QSIProject* project) {
	m_projectLoaded = project != nullptr;
	ui->menuProject->setEnabled(m_projectLoaded);
	ui->toolbarPlayGame->setEnabled(m_projectLoaded);
	ui->toolbarProjectProperties->setEnabled(m_projectLoaded);
	if(m_projectLoaded)
		m_project = project;

	updateTreeView();
}

void MainWindow::closeProject() {
	setCurrentProject(nullptr);
}

void MainWindow::handleModifiedFiles() {
	if(m_openEditors.count() == 0) return;
	ModifiedFilesDialog mfd(this);
	int num_modified = 0;
	QList<QTextEdit *> openEditors = ui->openFileTabs->findChildren<QTextEdit *>();

	for(int t = 0; t < openEditors.count(); t++) {
		/* if(openEditors.at(t)->document()->toPlainText() != m_openEditors.at(t)->text) {
			mfd.addModifiedItem(m_openEditors.at(t));
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
					openFile(fileAction->text());
				}
		);
	}
	settings.endArray();
}

void MainWindow::on_actionExit_triggered() {
	handleModifiedFiles();
	close(); // just temporary until I have a way to test if the currently open file has been modified
	//if(handleModifiedFiles() > 0) close();
}

void MainWindow::on_actionConfigure_QtSphere_IDE_triggered() {
	SettingsWindow settingsWindow(this);
	settingsWindow.setModal(true);
	settingsWindow.exec();
}

void MainWindow::on_toolbarSaveButton_triggered() {
	saveCurrentTab();
}

void MainWindow::on_toolbarOpenButton_triggered() {
	openFile();
}

void MainWindow::on_actionOpenFile_triggered() {
	openFile();
}

void MainWindow::on_openFileTabs_tabCloseRequested(int index) {
	SphereEditor* editor = m_openEditors.at(index);
	if(editor != nullptr) editor->closeFile();
	m_openEditors.removeAt(index);
	ui->openFileTabs->removeTab(index);
	delete editor;
}

void MainWindow::on_actionUndo_triggered() {
	SphereEditor* currentEditor = getCurrentEditor();
	if(currentEditor == nullptr) return;
	currentEditor->undo();

//	switch (currentEditor->editorType()) {
//	case SphereEditor::TextEditor: {
//		TextEditor* editor = dynamic_cast<TextEditor*>(currentEditor);
//		editor->undo();
//		break;
//	}
//	default:
//		break;
//	}
//	//ui->openFileTabs->currentWidget()->findChildren<QTextEdit *>().at(0)->undo();
}

void MainWindow::on_actionRedo_triggered() {
	SphereEditor* currentEditor = getCurrentEditor();
	if(currentEditor != nullptr) {
		qDebug() << "currentEditor type: " << currentEditor->editorType();
		currentEditor->redo();
	}
}

void MainWindow::on_toolbarProjectProperties_triggered() {
	if(openProjectProperties(false, m_project) == QDialog::Accepted)
		m_project->save();
}

void MainWindow::on_newProject_triggered() {
	ProjectPropertiesDialog propertiesDialog(true);
	if(propertiesDialog.exec() == QDialog::Accepted) {
		QDir newDir = QDir(propertiesDialog.getProject()->getPath(false));
		QString dirName = newDir.dirName();
		newDir.cdUp();
		qDebug() << newDir.absolutePath();
		if(!newDir.mkpath(dirName)) {
			errorBox("Error creating new project.");
			return;
		}
	}
}

void MainWindow::on_actionProject_Properties_triggered() {
	if(openProjectProperties(false, m_project, this) == QDialog::Accepted)
		m_project->save();
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
	TextEditor* newTextEdit = new TextEditor(this);
	newTextEdit->SphereEditor::setObjectName("textEdit" + QString::number(ui->openFileTabs->count()));
	newTextEdit->setTabIndex(ui->openFileTabs->insertTab(0, static_cast<QTextEdit*>(newTextEdit), "<Untitled>"));
	ui->openFileTabs->setTabToolTip(0, "<Untitled>");
	m_openEditors.append(newTextEdit);
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
	openProject(QFileDialog::getExistingDirectory(this,"Choose project path"));
}

void MainWindow::on_actionRefresh_triggered() {
	openProject(m_project->getPath(false));
}

void MainWindow::on_actionQSIGithub_triggered() {
	QDesktopServices::openUrl(QUrl("https://github.com/Eggbertx/QtSphere-IDE"));
}

void MainWindow::on_actionMSGithub_triggered() {
	QDesktopServices::openUrl(QUrl("https://github.com/fatcerberus/miniSphere"));
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index) {
	if(!m_projectLoaded) return;
	QFileSystemModel* model = dynamic_cast<QFileSystemModel*>(ui->treeView->model()); // new QFileSystemModel();
	model->setRootPath(m_project->getPath(false));
	if(model->fileInfo(index).isFile())
		openFile(model->filePath(index));
}

void MainWindow::checkCloseProjectOption() {
	if(!m_project || m_project->getPath(false) == "") ui->actionClose_Project->setEnabled(false);
	else ui->actionClose_Project->setEnabled(true);
}

void MainWindow::onProjectLoaded(QSIProject* project) {
	setCurrentProject(project);
}

void MainWindow::onEngineDropdownChanged(int index) {
	QSettings settings;
	switch (index) {
	case 0:
		setEngine("minisphere");
		settings.setValue("whichEngine", "minisphere");
		break;
	case 1:
		setEngine("legacy");
		settings.setValue("whichEngine", "legacy");
		break;
	}
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
		bool success = false;
		Spriteset* imported = Spriteset::fromImage(imagePath,dialog->getFrameSize(),
			dialog->removeDuplicatesChecked(), dialog->getTransparencyIn(), dialog->getTransparencyOut(),
			&success
		);
		if(!success) return;

		SpritesetEditor* ssView = new SpritesetEditor(this);
		ssView->attach(imported);
		QFileInfo fi = QFileInfo(imagePath);
		ssView->setTabIndex(ui->openFileTabs->insertTab(0, ssView, fi.fileName()));
		ui->openFileTabs->setCurrentIndex(0);
	}
}

void MainWindow::on_actionSave_triggered() {
	saveCurrentTab();
}

void MainWindow::on_actionStart_Page_triggered() {
	int numTabs = ui->openFileTabs->count();
	for(int t = 0; t < numTabs; t++) {
		if(QString(ui->openFileTabs->widget(t)->metaObject()->className()) == "StartPage") return;
	}
	ui->openFileTabs->addTab(m_startPage, "Start Page");
}

void MainWindow::on_taskListTable_customContextMenuRequested(const QPoint &pos) {
	QMenu* contextMenu = new QMenu();
	contextMenu->addAction("Open task list...");
	contextMenu->addAction("Import task list...");
	contextMenu->addAction("Save task list");
	contextMenu->addAction("Save task list as...");
	QWidget* child = ui->taskListTable->childAt(pos);
	if(child != nullptr) {
		QAction* result = contextMenu->exec(child->mapToGlobal(pos));
		if(result == nullptr) return;
		QString text = result->text();
		if(text == "Open task list...") {

		} else if(text == "Save task list") {
			m_taskList->saveList(false);
		} else if(text == "Save task list as...") {
			m_taskList->saveList(true);
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
	refreshRecentFiles();
}

void MainWindow::on_actionClose_Project_triggered() {
	closeProject();
}

void MainWindow::on_actionClose_triggered() {
	closeProject();
}

void MainWindow::on_toolbarPlayGame_triggered() {
	playGame(m_project->getBuildDir());
}

bool MainWindow::validEngineDirCheck() {
	QSettings settings;
	QString engineDir = settings.value("legacySphereDir").toString();
	if(engineDir == "") {
		errorBox("Engine directory must be set if the legacy engine is being used.");
		return false;
	}
	if(!QFile::exists(engineDir)) {
		errorBox("Engine directory set in QtSphereIDE settings does not exist.");
		return false;
	}
	return true;
}

void MainWindow::playGame(QString gameDir) {
	QSettings settings;
	QString minisphereDir = QDir(settings.value("minisphereDir").toString()).path();
	QString legacySphereDir = QDir(settings.value("legacySphereDir").toString()).path();
	QString whichEngine = settings.value("whichEngine", "minisphere").toString();

	QDir engineDir = QDir(settings.value("legacySphereDir").toString());

	if(!validEngineDirCheck()) return;
#if defined(Q_OS_UNIX)
	if(whichEngine == "legacy") {
		QProcess::startDetached("wine", QStringList({"./engine.exe", "-game", gameDir}), legacySphereDir);
	} else {
		QProcess::startDetached("minisphere", QStringList(gameDir), minisphereDir);
	}
#elif defined(Q_OS_WIN)
	if(whichEngine == "legacy") {
		//QProcess::startDetached("engine.exe", QStringList({"-game", gameDir}), legacySphereDir);
		QProcess::startDetached(
			"\"" + engineDir.filePath("engine.exe") + "\"",
			QStringList({"-game", gameDir}),
			engineDir.path()
		);
	} else {
		QProcess::startDetached("minisphere", QStringList({"."}), gameDir);
	}
#endif
}

void MainWindow::configureSphere() {
	QSettings settings;
	if(settings.value("whichEngine", "minisphere").toString() != "legacy") return;
	if(!validEngineDirCheck()) return;
	QDir engineDir = QDir(settings.value("legacySphereDir").toString());

#if defined(Q_OS_UNIX)
	QProcess::startDetached("wine", QStringList({"./config.exe"}), engineDir.path());
#elif defined(Q_OS_WIN)
	QProcess::startDetached("\"" + engineDir.filePath("config.exe") + "\"", {}, engineDir.path());
#endif
}

void MainWindow::on_actionLegacyConfig_triggered() {
	configureSphere();
}

void MainWindow::on_newMap_triggered() {
	NewMapDialog* dialog = new NewMapDialog(m_project, this);
	dialog->open();
}

void MainWindow::on_actionSystem_font_to_Sphere_font_triggered() {
	SphereFont* rfn = SphereFont::fromSystemFont(this);
	if(!rfn) return; // either the QFontDialog or the QColorDialog was cancelled.
}

void MainWindow::on_actionProject_Explorer_triggered() {
	ui->sideBar->setCurrentIndex(0);
}

void MainWindow::on_actionProject_Task_List_triggered() {
	ui->sideBar->setCurrentIndex(1);
}

void MainWindow::on_actionSound_Test_triggered() {
	ui->sideBar->setCurrentIndex(2);
}

void MainWindow::on_actionTiled_map_to_Sphere_map_triggered() {
	MapFile map(this);
	bool success = map.openTiledMap();
}
