#include <QDebug>

#include "projectpropertiesdialog.h"
#include "ui_projectpropertiesdialog.h"

ProjectPropertiesDialog::ProjectPropertiesDialog(QWidget *parent) : QDialog(parent),
	ui(new Ui::ProjectPropertiesDialog)
{
	qDebug() << "derp";
	ui->setupUi(this);
}

ProjectPropertiesDialog::~ProjectPropertiesDialog()
{
	delete ui;
}
