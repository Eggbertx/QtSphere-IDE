#include <QAction>
#include <QFileDialog>
#include <QString>

#include "newmapdialog.h"
#include "ui_newmapdialog.h"

NewMapDialog::NewMapDialog(QSIProject* project, QWidget *parent): QDialog(parent), ui(new Ui::NewMapDialog) {
	ui->setupUi(this);
	m_project = project;
	connect(ui->browse_btn, &QToolButton::clicked, this, &NewMapDialog::onBrowseButtonClicked);
}

NewMapDialog::~NewMapDialog() {
	disconnect(ui->browse_btn, &QToolButton::clicked, this, &NewMapDialog::onBrowseButtonClicked);
	delete ui;
}

QSize NewMapDialog::mapSize() {
	return QSize(ui->width_num->value(), ui->height_num->value());
}

QString NewMapDialog::tilesetPath() {
	return ui->browse_txt->text();
}

void NewMapDialog::setTilesetPath(QString path) {
	ui->browse_txt->setText(path);
}

void NewMapDialog::onBrowseButtonClicked() {
	QString fn;
	QString usePath = "";
	if(m_project && m_project->getPath(false) != "") {
		usePath = m_project->getPath(false);
	}
	fn = QFileDialog::getOpenFileName(this, "Open file", usePath,
		"Sphere tilesets (*.rts);;"
		"All files (*.*)"
	);
	if(fn == "") return;

	ui->browse_txt->setText(fn);
}
