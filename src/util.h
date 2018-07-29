#ifndef UTIL_H
#define UTIL_H

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QRegExp>
#include <QTextCharFormat>
#include <QEvent>

int handleModifiedFiles();

int question(QString title, QString message);

QString getWidgetType(QObject* widget);

void errorBox(QString message);

void warningBox(QString message);

void infoBox(QString info);

int readFile(QFile* file, void* into, int size);

int writeFile(QFile* file, void* from, int size);

QByteArray* imageBytes(QImage *image);

void replaceColor(QImage *imgIn, QColor colorIn, QColor colorOut);

QList<QImage> getUniqueImages(QList<QImage> list);

#endif // UTIL_H
