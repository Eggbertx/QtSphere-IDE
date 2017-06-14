#include <QFile>
#include <QFileInfo>
#include "objects/textfile.h"
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerhtml.h>

TextFile::TextFile(QWidget *parent) : QsciScintilla(parent) {
    QFontMetrics fontmetrics = QFontMetrics(this->font());
    this->setMarginsFont(this->font());
    this->setMarginWidth(0, fontmetrics.width(QString::number(this->lines()))+6);

    this->setMarginLineNumbers(0, true);
    this->setMarginsBackgroundColor(QColor("#cccccc"));
    connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    this->setCaretLineVisible(true);
    this->setCaretLineBackgroundColor(QColor("#ffe4e4"));

    QsciLexerJavaScript *lexer = new QsciLexerJavaScript();
    lexer->setDefaultFont(this->font());
    lexer->setFoldComments(true);
    this->setLexer(lexer);

    QsciScintilla::FoldStyle state = static_cast<QsciScintilla::FoldStyle>((!this->folding())*5);
    this->foldAll(state);
    this->setFolding(state);
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

void TextFile::readFile(QString filename) {
    QFile* file = new QFile(filename);
    QString fileExtension;
    if (!file->open(QIODevice::ReadWrite | QIODevice::Text)) return;
    this->fi = QFileInfo(filename);
    QByteArray bytes = file->readAll();
    this->setText(QString(bytes));
    if(this->fileExtension() == "js") {
        QsciLexerJavaScript *lexer = new QsciLexerJavaScript();
        lexer->setDefaultFont(this->font());
        lexer->setFoldComments(true);
        this->setLexer(lexer);
    } else if(this->fileExtension() == "html" || this->fileExtension() == "htm") {
        QsciLexerHTML *lexer = new QsciLexerHTML();
        lexer->setDefaultFont(this->font());
        lexer->setFoldCompact(true);
        this->setLexer(lexer);
    }
}

void TextFile::readFile(QFile file) {

}

void TextFile::onTextChanged() {
    QFontMetrics fontmetrics = this->fontMetrics();
    this->setMarginWidth(0, this->fontMetrics().width(QString::number(this->lines())) + 6);
}
