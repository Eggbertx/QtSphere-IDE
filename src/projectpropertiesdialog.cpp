#include <QDebug>
#include <QLineEdit>
#include <QToolButton>
#include <QFileDialog>

#include "projectpropertiesdialog.h"
#include "ui_projectpropertiesdialog.h"

ProjectPropertiesDialog::ProjectPropertiesDialog(bool newFile, QWidget *parent) : QDialog(parent), ui(new Ui::ProjectPropertiesDialog) {
	ui->setupUi(this);
    this->isNew = newFile;
    if(this->isNew) {
        this->setWindowTitle("New Project");
        ui->pathLineEdit->setEnabled(true);
        ui->pathButton->setEnabled(true);
    }
}

ProjectPropertiesDialog::~ProjectPropertiesDialog() {
	delete ui;
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
