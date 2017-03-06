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

void setTheme(QString theme);

bool handleEvents(QEvent e);

#endif // UTIL_H
