#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QProcess>
#include <QSettings>
#include <QStringList>
#include <QTextEdit>

#include "startpage.h"
#include "ui_startpage.h"
#include "mainwindow.h"
#include "qsiproject.h"
#include "util.h"

StartPage::StartPage(QWidget *parent) : QWidget(parent), ui(new Ui::StartPage) {
	ui->setupUi(this);
	this->baseInfoHTML = ui->gameInfoText->toHtml();
	QList<int> mainSplitterList;
	mainSplitterList << 250 << 100;
	ui->splitter->setSizes(mainSplitterList);
	this->setGameInfoText("","","","","");
	this->gameList = QList<QSIProject*>();
	ui->projectIcons->setContextMenuPolicy(Qt::CustomContextMenu);
	this->rightClickMenu = new QMenu(ui->projectIcons);
	this->startGameAction = this->rightClickMenu->addAction("Start game");
	this->loadProjectAction = this->rightClickMenu->addAction("Load project");
	this->openProjectDirAction = this->rightClickMenu->addAction("Open project directory");
	this->rightClickMenu->addSeparator();
	this->rightClickMenu->addAction("Refresh game list");

	this->refreshGameList();
}

StartPage::~StartPage() {
	delete ui;
}

QString StartPage::getGameInfoText() {
	return ui->gameInfoText->toHtml();
}

void StartPage::refreshGameList() {
	this->gameList.clear();
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
			QSIProject* qp = new QSIProject(dirs.at(d).absoluteFilePath());
			if(qp->name == "") {
				qp->name = dirs.at(d).fileName();
				qp->width = -1;
				qp->height = -1;
			}
			this->gameList << qp;
		}
	}
	settings.endArray();
	int listSize = this->gameList.length();
	for(int m = 0; m < listSize; m++) {
		QListWidgetItem* item = new QListWidgetItem(this->gameList.at(m)->name);
		item->setIcon(QIcon(":/icons/sphere-icon.png"));
		item->setSizeHint(QSize(120,64));
		ui->projectIcons->addItem(item);
		item->setTextAlignment(Qt::AlignCenter);
	}
}

void StartPage::setGameInfoText(QString name, QString author, QString resolution, QString path, QString description) {
	ui->gameInfoText->setHtml(this->baseInfoHTML.right(this->baseInfoHTML.length())
		.replace("[PROJECTNAME]", name)
		.replace("[PROJECTAUTHOR]", author)
		.replace("[PROJECTRESOLUTION]", resolution)
		.replace("[PROJECTPATH]", path)
		.replace("[PROJECTDESCRIPTION]", description)
	);
}

void StartPage::on_projectIcons_itemDoubleClicked(QListWidgetItem *item) {
	qDebug() << "Starting" << item->text();
}

void StartPage::on_projectIcons_itemClicked(QListWidgetItem *item) {
	int currentRow = ui->projectIcons->currentRow();
	QSIProject* currentProject = this->gameList.at(currentRow);

	if(currentProject != nullptr) {
		QString displayResolution = "";
		if(currentProject->width > 0 && currentProject->height > 0) {
			displayResolution = QString::number(currentProject->width) + "x" + QString::number(currentProject->height);
		}
		this->setGameInfoText(
			currentProject->name,
			currentProject->author,
			displayResolution,
			currentProject->getPath(false),
			currentProject->summary
		);
	}
}



void StartPage::on_projectIcons_customContextMenuRequested(const QPoint &pos) {
	QListWidgetItem* selected = ui->projectIcons->itemAt(pos);

	bool actionsEnabled = (selected != nullptr);
	this->startGameAction->setEnabled(actionsEnabled);
	this->loadProjectAction->setEnabled(actionsEnabled);
	this->openProjectDirAction->setEnabled(actionsEnabled);

	QAction* choice = this->rightClickMenu->exec(ui->projectIcons->mapToGlobal(pos));

	if(!choice) return;
	QString text = choice->text();
	if(text == "Start game") {

	} else if(text == "Load project")  {

	} else if(text == "Open project directory") {

	} else if(text == "Refresh game list") {
		this->refreshGameList();
	}
}
