#ifndef UTIL_H
#define UTIL_H

#include <QDebug>
#include <QMessageBox>
#include <QSyntaxHighlighter>
#include <QRegExp>
#include <QTextCharFormat>
#include <QEvent>

int handleModifiedFiles();

int question(QString title, QString message);

QString getWidgetType(QObject* widget);

void syntaxHighlighting(QWidget widget, bool yn);

bool handleEvents(QEvent e);

void errorBox(QString message);

void infoBox(QString info);

#endif // UTIL_H
