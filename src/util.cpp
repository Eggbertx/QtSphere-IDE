#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QApplication>
#include <QDataStream>
#include <QFile>
#include <QPainter>
#include <QByteArray>
#include <stdio.h>

#include "util.h"
#include "mainwindow.h"
#include "modifiedfilesdialog.h"
#include "formats/spherefile.h"


int handleModifiedFiles() {
	// if(numOpenFiles == 0) return 1;

	ModifiedFilesDialog m;
	m.exec();
	int clickResult = m.getResult();
	if(clickResult == 2) {
		// user clicked "Save All"
		/* for(openFille in openFiles) {
			openFile.save();
		} */
	}
	return clickResult;
}

QString getWidgetType(QObject* widget) {
	if(!widget->isWidgetType()) return "";
	return QString(widget->metaObject()->className());
}

void errorBox(QString message) {
	QMessageBox::critical(MainWindow::instance(), "Error!", message, QMessageBox::Ok);
}

void warningBox(QString message) {
	QMessageBox::warning(MainWindow::instance(), "Warning", message, QMessageBox::Ok);
}

void infoBox(QString info) {
	QMessageBox::information(MainWindow::instance(), "Info", info, QMessageBox::Ok);
}

int readFile(QFile* file, void* into, int size) {
	if(size < 1) return true;

	/*if(!file->exists()) {
		errorBox(file->fileName() +" doesn't exist!");
		return false;
	}*/
	if(!file->isOpen()){
		if(!file->open(QIODevice::ReadOnly)) {
			errorBox("Failed opening " + file->fileName() + " for reading");
			return false;
		}
	}
	return file->read((char*)into, size);
}

int writeFile(QFile *file, void *from, int size) {
	if(size < 1) return true;
	if(!file->isOpen()) {
		if(!file->open(QIODevice::WriteOnly)) {
			errorBox("Failed opening " + file->fileName() + " for writing");
			return false;
		}
	}
	int numBytes = file->write(reinterpret_cast<char*>(from), size);
	file->flush();
	return numBytes;
}

void skipBytes(QFile* file, const int numBytes) {
	uint8_t* dump = new uint8_t[numBytes];
	file->read((char*)dump, numBytes);
}

void replaceColor(QImage *imgIn, QColor colorIn, QColor colorOut) {
	int width = imgIn->width();
	int height = imgIn->height();
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			if(imgIn->pixelColor(x,y) == colorIn) {
				imgIn->setPixelColor(x,y,colorOut);
			}
		}
	}
}

QByteArray* imageBytes(QImage *image) {
	QByteArray* ba = new QByteArray();
	int width = image->width();
	int height = image->height();

	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			ba->append((char)image->pixelColor(x,y).red());
			ba->append((char)image->pixelColor(x,y).green());
			ba->append((char)image->pixelColor(x,y).blue());
			ba->append((char)image->pixelColor(x,y).alpha());
		}
	}
	return ba;
}

QList<QImage> getUniqueImages(QList<QImage> list) {
	QList<QImage> uniqueImages;

	foreach(QImage image, list) {
		if(!uniqueImages.contains(image))
			uniqueImages.append(image);
	}
	return uniqueImages;
}
