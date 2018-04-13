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
	}
	this->project = project;
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
