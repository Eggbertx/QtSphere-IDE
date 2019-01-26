#include "entityeditdialog.h"
#include "ui_entityeditdialog.h"

EntityEditDialog::EntityEditDialog(QWidget *parent): QDialog(parent), ui(new Ui::EntityEditDialog) {
	ui->setupUi(this);
	this->attached = false;
}

EntityEditDialog::~EntityEditDialog() {
	delete ui;
}

QString EntityEditDialog::getScript(int which) {
	switch (which) {
		case 0:
			return this->onCreateScript;
		case 1:
			return this->onDestroyScript;
		case 2:
			return this->onTouchScript;
		case 3:
			return this->onTalkScript;
		case 4:
			return this->onGenerateCommandsScript;
	}
	return "";
}

void EntityEditDialog::attachMap(MapFile* map) {
	this->map = map;
	this->attached = true;
}

void EntityEditDialog::on_nameGenerate_btn_released() {
	QString spriteset_fn = ui->spriteset_txt->text();
	if(!this->attached || spriteset_fn == "") return;
	QString spriteset_noExt = ui->spriteset_txt->text().remove(spriteset_fn.lastIndexOf(".rss"),4);

	QString suffix = "_";
	QList<MapFile::entity> entities = this->map->getEntities();
	int numWithName = 1;
	foreach(MapFile::entity mapEntity, entities) {

		if(mapEntity.name == spriteset_noExt) numWithName++;
	}
}
