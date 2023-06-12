#ifndef UTIL_H
#define UTIL_H

#include <QEvent>
#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextCharFormat>

int handleModifiedFiles();

QString getWidgetType(QObject* widget);

void errorBox(QString message);

void warningBox(QString message);

void infoBox(QString info);

int readFile(QFile* file, void* into, int size);

int writeFile(QFile* file, const void *from, int size);

void skipBytes(QFile* file, const int numBytes);

QByteArray* imageBytes(QImage* image);

void replaceColor(QImage* imgIn, QColor colorIn, QColor colorOut);

QList<QImage> getUniqueImages(QList<QImage> list);

inline const char* toString(QString str) {
	return str.toStdString().c_str();
}

#endif // UTIL_H
