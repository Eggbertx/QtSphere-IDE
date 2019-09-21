#include <QByteArray>
#include <QDesktopServices>
#include <QDir>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QProcess>
#include <QSettings>
#include <QStringList>
#include <QTextEdit>

#include "widgets/startpage.h"
#include "ui_startpage.h"
#include "mainwindow.h"
#include "qsiproject.h"
#include "util.h"

StartPage::StartPage(QWidget *parent) : QWidget(parent), ui(new Ui::StartPage) {
	ui->setupUi(this);
	m_baseInfoHTML = ui->gameInfoText->toHtml();
	QList<int> mainSplitterList;
	mainSplitterList << 250 << 100;
	ui->splitter->setSizes(mainSplitterList);
	setGameInfoText("","","","","");
	m_gameList = QList<QSIProject*>();
	ui->projectIcons->setContextMenuPolicy(Qt::CustomContextMenu);
	m_rightClickMenu = new QMenu(ui->projectIcons);


	m_startGameAction = m_rightClickMenu->addAction("Start game");
	m_loadProjectAction = m_rightClickMenu->addAction("Load project");
	m_openProjectDirAction = m_rightClickMenu->addAction("Open project directory");
	m_rightClickMenu->addSeparator();
	m_rightClickMenu->addAction("Refresh game list");

	refreshGameList();
}

StartPage::~StartPage() {
	delete ui;
}

QString StartPage::getGameInfoText() {
	return ui->gameInfoText->toHtml();
}

void StartPage::refreshGameList() {
	m_gameList.clear();
	ui->projectIcons->clear();
	QStringList projectDirs;
	QSettings settings;
	int numProjectDirs = settings.beginReadArray("projectDirs");

	for(int p = 0; p < numProjectDirs; ++p) {
		settings.setArrayIndex(p);
		QDir dir(settings.value("directory").toString());

		if(!dir.exists() || dir.isEmpty()) return;
		QFileInfoList dirs = dir.entryInfoList(QDir::NoDotAndDotDot|QDir::Dirs);

		int numDirs = dirs.length();
		for(int d = 0; d < numDirs; d++) {
			QSIProject* project = new QSIProject();
			if(project->open(dirs.at(d).absoluteFilePath())) {
				project->setName(dirs.at(d).fileName());
				project->setWidth(-1);
				project->setWidth(-1);
				m_gameList << project;
			}
		}
	}
	settings.endArray();

	foreach (QSIProject* project, m_gameList) {
		QListWidgetItem* item = new QListWidgetItem(project->getIcon(), project->getName());
		item->setTextAlignment(Qt::AlignCenter);
		item->setSizeHint(QSize(150,80));
		ui->projectIcons->addItem(item);
	}
}

void StartPage::setGameInfoText(QString name, QString author, QString resolution, QString path, QString description) {
	ui->gameInfoText->setHtml(m_baseInfoHTML.right(m_baseInfoHTML.length())
		.replace("[PROJECTNAME]", name)
		.replace("[PROJECTAUTHOR]", author)
		.replace("[PROJECTRESOLUTION]", resolution)
		.replace("[PROJECTPATH]", path)
		.replace("[PROJECTDESCRIPTION]", description)
	);
}

void StartPage::on_projectIcons_customContextMenuRequested(const QPoint &pos) {
	QListWidgetItem* selected = ui->projectIcons->itemAt(pos);

	bool actionsEnabled = (selected != nullptr);
	m_startGameAction->setEnabled(false);
	m_loadProjectAction->setEnabled(actionsEnabled);
	m_openProjectDirAction->setEnabled(actionsEnabled);

	QAction* choice = m_rightClickMenu->exec(ui->projectIcons->mapToGlobal(pos));

	if(!choice || !m_currentProject) return;
	QString text = choice->text();
	if(text == "Start game") {

	} else if(text == "Load project")  {
		MainWindow::instance()->openProject(m_gameList.at(ui->projectIcons->currentRow())->getPath(true));
	} else if(text == "Open project directory") {
		QDesktopServices::openUrl(QUrl(m_currentProject->getPath(false)).path());
	} else if(text == "Refresh game list") {
		refreshGameList();
	}
}

void StartPage::on_projectIcons_itemActivated(QListWidgetItem *item) {
	MainWindow::instance()->openProject(m_gameList.at(ui->projectIcons->currentRow())->getPath(true));
	emit projectLoaded(m_currentProject);
}

void StartPage::on_projectIcons_itemSelectionChanged() {
	int currentRow = ui->projectIcons->currentRow();
	if(m_gameList.length() == 0) return;
	m_currentProject = m_gameList.at(currentRow);
	if(m_currentProject == nullptr) return;

	QString displayResolution = "";
	if(m_currentProject->getWidth() > 0 && m_currentProject->getHeight() > 0) {
		displayResolution = QString::number(m_currentProject->getWidth()) + "x" + QString::number(m_currentProject->getHeight());
	}
	setGameInfoText(
		m_currentProject->getName(),
		m_currentProject->getAuthor(),
		displayResolution,
		m_currentProject->getPath(false),
		m_currentProject->getSummary()
	);
}
