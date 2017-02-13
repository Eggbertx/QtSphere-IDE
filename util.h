#ifndef UTIL_H
#define UTIL_H

#include <QDebug>
#include <QMessageBox>


int handleModifiedFiles();

int question(QString title, QString message);

QString getWidgetType(QObject* widget);

void setTheme(QString theme);

#endif // UTIL_H
