#include <QProcess>
#include <QFileDialog>
#include <QMenu>
#include <QCloseEvent>
#include <QTime>
#include <QTextStream>
#include <QComboBox>
#include <assert.h>

#include "mainwindow.h"
#include "util.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "settingswindow.h"
#include "objects/qsifile.h"
#include "texteffects.h"
#include "modifiedfilesdialog.h"

MainWindow* MainWindow::_instance = NULL;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    Q_ASSERT(!_instance);
    _instance = this;
    ui->setupUi(this);

    QComboBox *syntaxes = new QComboBox();
    QStringList syntaxes_str;
    syntaxes_str << "Plaintext" << "Javascript" << "TypeScript" << "SGM Project File" << "JSON";
    syntaxes->addItems(syntaxes_str);
    syntaxes->setStyleSheet("margin-right: 8px;");


    ui->statusBar->addWidget(new QLabel("Ready."));
    ui->statusBar->addPermanentWidget(syntaxes);

	ui->toolbarNewButton->setIcon(this->style()->standardIcon(QStyle::SP_FileIcon));
	ui->toolbarOpenButton->setIcon(this->style()->standardIcon(QStyle::SP_DialogOpenButton));
	ui->toolbarSaveButton->setIcon(this->style()->standardIcon(QStyle::SP_DriveFDIcon));

	ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

	QTableWidgetItem* h = new QTableWidgetItem("Task");
	h->setCheckState(Qt::Unchecked);

	ui->tableWidget->setItem(0, 1, h);

	ui->centralWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->centralWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(showContextMenu(const QPoint&)));

	ui->openFileTabs->setTabsClosable(true);
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
    foreach(QObject* child, ui->statusBar->children()) {
        if(getWidgetType(child) == "QLabel") {
            QLabel* lbl = qobject_cast<QLabel *>(child);
            lbl->setText(status);
        }
    }
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
    /*event->accept(); // leaving this here until we can reliably check to see if an open file has been midified
	event->ignore();
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
	QObjectList tabChildren = ui->openFileTabs->currentWidget()->children();
    //ui->openFileTabs->current
    foreach(QObject* tab, tabChildren) {
		if(getWidgetType(tab) == "QTextEdit") {
			QTextEdit* textdata = qobject_cast<QTextEdit*>(tab);
			QString saveFileName = QFileDialog::getSaveFileName(this, "Save script", "","Script (*.js);;Text file (*.txt);;All files (*)");
			QFile saveFile(saveFileName);
			if(!saveFile.open(QIODevice::Text|QIODevice::WriteOnly)) {
				this->console("Failed saving file: " + saveFile.errorString());
				return;
			} else {
				QTextStream out(&saveFile);
				out.setCodec("UTF-8");
                out << QString::fromUtf8(textdata->document()->toPlainText().toStdString().c_str());
				saveFile.flush();
				saveFile.close();
			}
		}
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
				editor->setTabStopWidth(40);
				highlighter = new TextEffects(editor->document());
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

void MainWindow::openFile(QString fileName) {
	QString fn;
	if(fileName == "") {
		fn = QFileDialog::getOpenFileName(this, "Open file", QDir::currentPath(),
												"All supported files (*.sgm *.txt *.js *.ts *.rmp *.rss *.rws);;"
												"Sphere projects (*.sgm);;"
												"Script files (*.js *.ts *.coffee);;"
                                                "Text files (*.txt);;"
												"All files (*.*)");
	} else {
		fn = fileName;
	}

    QTextEdit *qte = new QTextEdit(ui->openFileTabs);
    QSIFile *qsifile = new QSIFile(fn, qte);
    QTextOption options = qsifile->textEditor()->document()->defaultTextOption();
    options.setTabStop(24);
    qsifile->textEditor()->document()->setDefaultTextOption(options);

    qsifile->textEditor()->setPlainText(qsifile->text);

    if(qsifile->fileType == QSIFile::JavaScript || qsifile->fileType == QSIFile::TypeScript) {
        highlighter = new TextEffects(qsifile->textEditor()->document());
        this->setupEditor();
    }

    ui->openFileTabs->insertTab(0,qte,qsifile->baseName());
    openFiles.insert(0,qsifile);
	ui->openFileTabs->setCurrentIndex(0);

}

void MainWindow::prepareForText() {

}

void MainWindow::prepareForCode() {

}

void MainWindow::openProject(QString fileName) {
    this->console("Loading project: " + fileName);
}

void MainWindow::handleModifiedFiles() {
    ModifiedFilesDialog mfd(this);
    int num_modified;
    for(int t = 0; t < ui->openFileTabs->count(); t++) {
        QWidget *tab = ui->openFileTabs->widget(t);
        if(getWidgetType(tab) == "QTextEdit") {
            QTextEdit *qte = dynamic_cast<QTextEdit *>(tab);
            if(qte->document()->toPlainText() != this->openFiles.at(t)->text) {
                mfd.addModifiedItem(this->openFiles.at(t));
                num_modified++;
            }
        }
    }
    if(num_modified > 0)
        mfd.exec();
}

void MainWindow::on_actionExit_triggered() {
    handleModifiedFiles();
    this->close(); // just temporary until I have a way to test if the currently open file has been modified
    //if(handleModifiedFiles() > 0) this->close();
}


void MainWindow::on_actionOpen_triggered() {
	this->openFile();
}

void MainWindow::on_actionConfigure_QtSphere_IDE_triggered() {
    SettingsWindow settingsWindow(this);
    settingsWindow.setModal(true);
    settingsWindow.exec();
}

void MainWindow::on_toolbarNewButton_triggered() {
    QTextEdit* newTextEdit = new QTextEdit("", ui->openFileTabs);
    this->openFiles.append(new QSIFile("",newTextEdit));
	newTextEdit->setObjectName("textEdit" + QString::number(ui->openFileTabs->count()) );
	highlighter = new TextEffects(newTextEdit->document());
	ui->openFileTabs->insertTab(0, newTextEdit, "<Untitled>");
	this->setupEditor();
}

void MainWindow::on_toolbarSaveButton_triggered() {
	this->saveCurrentTab();
}

void MainWindow::on_toolbarOpenButton_triggered() {
    this->openFile();
}

void MainWindow::on_openFileTabs_tabCloseRequested(int index) {
    this->openFiles.remove(index);
    ui->openFileTabs->removeTab(index);
}
