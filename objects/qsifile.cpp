#include "qsifile.h"

QSIFile::QSIFile(QFile *parent) : QFile(parent) {
    QString fileExtension;

}

QSIFile::QSIFile(QString fn) {
    QString fileExtension;
    if (!this->open(QIODevice::ReadWrite | QIODevice::Text)) return;
    QByteArray bytes = this->readAll();
    this->setText(bytes);

}

QString QSIFile::getText() {
    return this->fileText;
}

void QSIFile::setText(QString txt) {
    this->fileText = txt;
}
