#include <QLineEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QToolButton>
#include <QFileDialog>

#include "dialogs/projectpropertiesdialog.h"
#include "ui_projectpropertiesdialog.h"
#include "qsiproject.h"

ProjectPropertiesDialog::ProjectPropertiesDialog(bool newFile, QSIProject* project, QWidget *parent) : QDialog(parent), ui(new Ui::ProjectPropertiesDialog) {
	ui->setupUi(this);
	m_isNew = newFile;
	if(m_isNew) {
		setWindowTitle("New Project");
		ui->pathLineEdit->setEnabled(true);
		ui->pathButton->setEnabled(true);
		m_project = new QSIProject(this);
	} else {
		m_project = project;
		ui->pathLineEdit->setEnabled(false);
		ui->pathButton->setEnabled(false);
		if(!project) return;
		ui->pathLineEdit->setText(project->getPath(false));
		ui->nameLineEdit->setText(project->getName());
		ui->authorLineEdit->setText(project->getAuthor());
		ui->reswLineEdit->setText(QString::number(project->getWidth()));
		ui->reshLineEdit->setText(QString::number(project->getHeight()));
		ui->summaryText->document()->setPlainText(project->getSummary());
		ui->buildInLineEdit->setText(project->getBuildDir());
		ui->entryScriptLineEdit->setText(project->getMainScript());
		if(project->getCompiler() == "Cell") ui->compilerCB->setCurrentIndex(0);
		else ui->compilerCB->setCurrentIndex(1);
	}
	connect(ui->resolutionCBox, &QComboBox::currentTextChanged, this, &ProjectPropertiesDialog::onResolutionCBoxCurrentTextChanged);
	connect(ui->pathButton, &QToolButton::clicked, this, &ProjectPropertiesDialog::onPathButtonClicked);
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ProjectPropertiesDialog::onButtonBoxAccepted);
}

ProjectPropertiesDialog::~ProjectPropertiesDialog() {
	disconnect(ui->resolutionCBox, &QComboBox::currentTextChanged, this, &ProjectPropertiesDialog::onResolutionCBoxCurrentTextChanged);
	disconnect(ui->pathButton, &QToolButton::clicked, this, &ProjectPropertiesDialog::onPathButtonClicked);
	disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ProjectPropertiesDialog::onButtonBoxAccepted);
	delete ui;
}

QSIProject* ProjectPropertiesDialog::getProject() {
	return m_project;
}

void ProjectPropertiesDialog::onResolutionCBoxCurrentTextChanged(const QString &newText) {
	QStringList resArr = newText.split('x');
	if(resArr.size() != 2) return;
	ui->reswLineEdit->setText(resArr.at(0));
	ui->reshLineEdit->setText(resArr.at(1));
}

void ProjectPropertiesDialog::onPathButtonClicked() {
	ui->pathLineEdit->setText(QFileDialog::getExistingDirectory(parentWidget(),"Choose project path"));
}

void ProjectPropertiesDialog::onButtonBoxAccepted() {
	m_project->setPath(ui->pathLineEdit->text(), false);
	m_project->setName(ui->nameLineEdit->text());
	m_project->setAuthor(ui->authorLineEdit->text());
	m_project->setSize(ui->reswLineEdit->text().toInt(), ui->reshLineEdit->text().toInt());
	m_project->setSummary(ui->summaryText->document()->toPlainText());
	m_project->setMainScript(ui->entryScriptLineEdit->text());
	m_project->setBuildDir(ui->buildInLineEdit->text());
	m_project->setCompiler(ui->compilerCB->currentText());
	m_project->save();
}
