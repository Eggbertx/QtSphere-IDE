#include "fonteditor.h"
#include "ui_fonteditor.h"

// This doesn't do anything yet, and with the direction
// I'm planning on taking QSI, it might be removed.
FontEditor::FontEditor(QWidget *parent): QWidget(parent), ui(new Ui::FontEditor) {
	ui->setupUi(this);
}

FontEditor::~FontEditor() {
	delete ui;
}
