#include <QFile>
#include <QFileInfo>
#include "objects/textfile.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerhtml.h>

TextFile::TextFile(QWidget *parent) : QWidget() {
	this->textWidget = new QsciScintilla(parent);

	QFontMetrics fontmetrics = QFontMetrics(this->textWidget->font());
	this->textWidget->setMarginsFont(this->textWidget->font());
	this->textWidget->setMarginWidth(0, fontmetrics.width(QString::number(this->textWidget->lines()))+6);

	this->textWidget->setMarginLineNumbers(0, true);
	this->textWidget->setMarginsBackgroundColor(QColor("#cccccc"));
    connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

	this->textWidget->setCaretLineVisible(true);
	this->textWidget->setCaretLineBackgroundColor(QColor("#ffe4e4"));

    QsciLexerJavaScript *lexer = new QsciLexerJavaScript();
	lexer->setDefaultFont(this->textWidget->font());
    lexer->setFoldComments(true);
	this->textWidget->setLexer(lexer);

	QsciScintilla::FoldStyle state = static_cast<QsciScintilla::FoldStyle>((!this->textWidget->folding())*5);
	this->textWidget->foldAll(state);
	this->textWidget->setFolding(state);
}

QFileInfo TextFile::fileInfo() {
    return this->fi;
}

QString TextFile::fileName() {
    return this->fi.fileName();
}

QString TextFile::fileExtension() {
    return this->fi.suffix();
}

QsciScintilla* TextFile::getTextWidget() {
	return this->textWidget;
}

void TextFile::readFile(QString filename) {
	QFile* file = new QFile(filename);
    QString fileExtension;
    if (!file->open(QIODevice::ReadWrite | QIODevice::Text)) return;
    this->fi = QFileInfo(filename);
    QByteArray bytes = file->readAll();
	this->textWidget->setText(QString(bytes));
    if(this->fileExtension() == "js") {
        QsciLexerJavaScript *lexer = new QsciLexerJavaScript();
		lexer->setDefaultFont(this->textWidget->font());
        lexer->setFoldComments(true);
		this->textWidget->setLexer(lexer);
    } else if(this->fileExtension() == "html" || this->fileExtension() == "htm") {
        QsciLexerHTML *lexer = new QsciLexerHTML();
		lexer->setDefaultFont(this->textWidget->font());
        lexer->setFoldCompact(true);
		this->textWidget->setLexer(lexer);
	}
}

void TextFile::readFile(QFile file) {

}

void TextFile::onTextChanged() {
	//QFontMetrics fontmetrics = this->fontMetrics();
	//this->setMarginWidth(0, this->fontMetrics().width(QString::number(this->lines())) + 6);
}
