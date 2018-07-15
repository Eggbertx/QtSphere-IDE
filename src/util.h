#ifndef UTIL_H
#define UTIL_H

#include <QDebug>
#include <QMessageBox>
#include <QRegExp>
#include <QTextCharFormat>
#include <QEvent>

int handleModifiedFiles();

int question(QString title, QString message);

QString getWidgetType(QObject* widget);

void errorBox(QString message);

void infoBox(QString info);

bool readFile(QString filename, void* into, int size, int offset);

#endif // UTIL_H
