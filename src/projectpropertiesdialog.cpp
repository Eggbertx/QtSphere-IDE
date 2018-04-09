#include <QDebug>
#include <QLineEdit>
#include <QToolButton>
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

void ProjectPropertiesDialog::on_pathButton_triggered(QAction *arg1) {
    if(!this->isNew) return;

}

void ProjectPropertiesDialog::on_resolutionCBox_currentTextChanged(const QString &newText) {
    QStringList resArr = newText.split('x');
    if(resArr.size() != 2) return;
    int width = resArr.at(0).toInt();
    int height = resArr.at(1).toInt();
}
