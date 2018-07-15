#include <QDebug>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QToolButton>
#include <QFileDialog>

#include "projectpropertiesdialog.h"
#include "ui_projectpropertiesdialog.h"
#include "qsiproject.h"

ProjectPropertiesDialog::ProjectPropertiesDialog(bool newFile, QSIProject* project, QWidget *parent) : QDialog(parent), ui(new Ui::ProjectPropertiesDialog) {
	ui->setupUi(this);
	this->isNew = newFile;
	if(this->isNew) {
		this->setWindowTitle("New Project");
		ui->pathLineEdit->setEnabled(true);
		ui->pathButton->setEnabled(true);
		this->project = new QSIProject("", this);
	} else {
		this->project = project;
		ui->pathLineEdit->setEnabled(false);
		ui->pathButton->setEnabled(false);
		if(!project) return;
		ui->pathLineEdit->setText(project->getPath());
		ui->nameLineEdit->setText(project->name);
		ui->authorLineEdit->setText(project->author);
		ui->reswLineEdit->setText(QString::number(project->width));
		ui->reshLineEdit->setText(QString::number(project->height));
		ui->summaryText->document()->setPlainText(project->summary);
		ui->buildInLineEdit->setText(project->buildDir);
		ui->entryScriptLineEdit->setText(project->script);
		if(project->getCompiler() == "Vanilla") ui->compilerCB->setCurrentIndex(0);
		else ui->compilerCB->setCurrentIndex(1);
	}
	if(this->project) this->project = new QSIProject(""); // just in case
}

ProjectPropertiesDialog::~ProjectPropertiesDialog() {
	delete ui;
}

QSIProject* ProjectPropertiesDialog::getProject() {
	return this->project;
}

bool ProjectPropertiesDialog::writeSGMFile() {
	return true;
}

void ProjectPropertiesDialog::on_resolutionCBox_currentTextChanged(const QString &newText) {
	QStringList resArr = newText.split('x');
	if(resArr.size() != 2) return;
	ui->reswLineEdit->setText(resArr.at(0));
	ui->reshLineEdit->setText(resArr.at(1));
}

void ProjectPropertiesDialog::on_pathButton_clicked() {
	ui->pathLineEdit->setText(QFileDialog::getExistingDirectory(this->parentWidget(),"Choose project path"));
}

void ProjectPropertiesDialog::on_buttonBox_accepted() {
	this->project->name = ui->nameLineEdit->text();
	this->project->author = ui->authorLineEdit->text();
	this->project->width = ui->reswLineEdit->text().toInt();
	this->project->height = ui->reshLineEdit->text().toInt();
	this->project->summary = ui->summaryText->document()->toPlainText();
	this->project->script = ui->entryScriptLineEdit->text();
	this->project->buildDir = ui->buildInLineEdit->text();
	this->project->setCompiler(ui->compilerCB->currentText());
}
