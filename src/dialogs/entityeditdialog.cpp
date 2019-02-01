#include "dialogs/entityeditdialog.h"
#include "ui_entityeditdialog.h"

EntityEditDialog::EntityEditDialog(QWidget *parent): QDialog(parent), ui(new Ui::EntityEditDialog) {
	ui->setupUi(this);
	m_attached = false;
}

EntityEditDialog::~EntityEditDialog() {
	delete ui;
}

QString EntityEditDialog::getScript(int which) {
	switch (which) {
		case 0:
			return m_onCreateScript;
		case 1:
			return m_onDestroyScript;
		case 2:
			return m_onTouchScript;
		case 3:
			return m_onTalkScript;
		case 4:
			return m_onGenerateCommandsScript;
	}
	return "";
}

void EntityEditDialog::attachMap(MapFile* map) {
	m_map = map;
	m_attached = true;
}

void EntityEditDialog::on_nameGenerate_btn_released() {
	QString spriteset_fn = ui->spriteset_txt->text();
	if(!m_attached || spriteset_fn == "") return;
	QString spriteset_noExt = ui->spriteset_txt->text().remove(spriteset_fn.lastIndexOf(".rss"),4);

	QString suffix = "_";
	QList<MapFile::entity> entities = m_map->getEntities();
	int numWithName = 1;
	foreach(MapFile::entity mapEntity, entities) {
		if(mapEntity.name == spriteset_noExt) numWithName++;
	}
}
