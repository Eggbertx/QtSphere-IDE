#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

#include "qsifile.h"
#include "mainwindow.h"

QSIFile::QSIFile(QString fn, QTextEdit *parentWidget) : QFile(fn) {
    QString fileExtension;
    if(fn == "") {
        // this is a new file, don't try to load anything
        this->_file = new QFile();
        this->textEditWidget = parentWidget;
        fileExtension = "";
    } else {
        fileExtension = QFileInfo(fn).suffix();
        this->_file = new QFile(fn);
        this->textEditWidget = parentWidget;
        if(fileExtension == "js" || fileExtension == "ts") {
            this->fileType = QSIFile::JavaScript;
            MainWindow::instance()->console("Loading " + fn + " (Javascript)");
        }
        if(!this->file()->open(QIODevice::ReadWrite)) {
            QMessageBox::critical(MainWindow::instance(), "ERROR", "Failed to load " + this->fileName() + ": " + this->file()->errorString(), QMessageBox::Ok);
            return;
        } else {
            this->text = this->_file->readAll();
            this->loadedStatus = true;
        }
    }
}

QFile* QSIFile::file() {
    return this->_file;
}

bool QSIFile::isModified(QTextEdit *editor) {
    return editor->document()->toPlainText() == this->text;
}

QString QSIFile::baseName() {
    return this->fileName().split("/").last();
}

QTextEdit* QSIFile::textEditor() {
    return this->textEditWidget;
}

bool QSIFile::isLoaded() {
	return this->loadedStatus;
}

void QSIFile::setLoaded() {
	this->loadedStatus = true;
}
