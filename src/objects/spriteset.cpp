#include <QBitmap>
#include <QBuffer>
#include <QByteArray>
#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPoint>

#include "mainwindow.h"
#include "spriteset.h"
#include "util.h"
#include "objects/spherefile.h"
#include "importoptionsdialog.h"

Spriteset::Spriteset(QObject *parent) : SphereFile(parent) {}

Spriteset::~Spriteset() {
	delete this->file;
}

Spriteset* Spriteset::fromImage(QString filename, QSize frameSize, bool removeDuplicates, QColor inColor, QColor outColor) {
	Spriteset* newSS = new Spriteset();
	newSS->valid = false;

	QImage* importImg = new QImage(filename);

	int frameW = frameSize.width();
	int frameH = frameSize.height();

	replaceColor(importImg, inColor, outColor);

	if(importImg->width() % frameW != 0
	|| importImg->height() % frameH != 0) {
		errorBox("Error importing " + filename + "(invalid frame size)");
		delete importImg;
		return newSS;
	}
	newSS = new Spriteset();

	newSS->images = QList<QImage>();
	int numFramesX = importImg->width()/frameW;
	int numFramesY = importImg->height()/frameH;

	newSS->directions = QList<SSDirection>();

	for(int y = 0; y < numFramesY; y++) {
		for(int x = 0; x < numFramesX; x++) {
			QImage image = importImg->copy(x*frameW,y*frameH,frameW,frameH);

			if((removeDuplicates && !newSS->images.contains(image)) || !removeDuplicates) {
				newSS->images.append(QImage(image));
			}
		}
	}

	rss_header header;
	memcpy(header.signature, ".rss", 4);
	header.version = 3;
	header.num_images = newSS->images.size();
	header.frame_width = frameW;
	header.frame_height = frameH;
	header.num_directions = 4;
	header.base_x1 = 0;
	header.base_y1 = 0;
	header.base_x2 = 0;
	header.base_y2 = 0;
	newSS->header = header;

	newSS->addDirection("north", 3);
	newSS->addDirection("east", 3);
	newSS->addDirection("south", 3);
	newSS->addDirection("west", 3);
	newSS->valid = true;
	return newSS;
}


void Spriteset::createNew() {
	this->filename = "Untitled.rss";
}

bool Spriteset::open(QString filename) {
	qDebug() << "opening" << filename << "from spriteset.cpp";
	this->filename = (char*)filename.toStdString().c_str();
	this->file = new QFile(this->filename);

	if(!this->file->exists()) {
		errorBox("The spriteset " + filename + " doesn't exist!");
		return false;
	}
	if(!this->file->open(QIODevice::ReadOnly)) {
		errorBox("Failed loading spriteset " + filename);
		this->file->close();
		return false;
	}
	readFile(this->file, &this->header, sizeof(header));
	if (memcmp(header.signature, ".rss", 4) != 0) {
		errorBox("Error: " + QString(filename) + " is not a valid spriteset!");
		this->file->close();
		return false;
	}

	switch(header.version) {
	case 1:
	case 2:
		errorBox("v1 and v2 spritesets are not currently supported.");
		this->file->close();
		return false;
		break;
	case 3: {
		this->images = QList<QImage>();
		for(int i = 0; i < header.num_images; i++) {
			const int num_img_bytes = header.frame_width*header.frame_height*4;
			unsigned char image_bytes[num_img_bytes];
			readFile(this->file, &image_bytes, num_img_bytes);

			this->images.append(QImage(
				image_bytes,
				header.frame_width, header.frame_height,
				QImage::Format_RGBA8888
			).copy(0,0,header.frame_width,header.frame_height));
		}

		this->directions = QList<SSDirection>();
		for(int d = 0; d < header.num_directions; d++) {
			SSDirection direction = SSDirection();
			uint16_t num_frames;
			readFile(this->file, &num_frames, sizeof(num_frames));
			this->file->skip(6);

			uint16_t str_length;
			readFile(this->file, &str_length, sizeof(str_length));

			const uint16_t str_length_const = str_length; // because MSVC loves to be cantankerous with non-constant array indices
			char direction_name[str_length_const];
			readFile(this->file, direction_name, str_length);
			direction.name = QString(direction_name);

			direction.frames = QList<SSFrame>();
			for(int f = 0; f < num_frames;f++) {
				SSFrame frame = SSFrame();
				readFile(this->file, &frame, sizeof(frame));
				direction.frames.append(frame);
			}

			this->directions.append(direction);
		}
		break;
	}
	default:
		errorBox("Invalid spriteset version: " + QString::number(this->header.version));
		this->file->close();
		return false;
		break;
	}
	this->file->close();
	delete this->file;
	return true;
}

bool Spriteset::save(QString filename) {
	QFile* outFile = new QFile(filename);
	memset(&this->header.reserved[0], 0, sizeof(this->header.reserved));

	writeFile(outFile, &this->header, sizeof(this->header));

	switch(this->header.version) {
	case 1:
	case 2:
		//this shouldn't technically be possible
		errorBox("v1 and v2 spritesets are not currently supported.");
		outFile->close();
		return false;
		break;
	case 3: {
		foreach(QImage image, this->images) {
			QByteArray* bytes = imageBytes(&image);
			writeFile(outFile, bytes->data(), bytes->length());
		}

		foreach(SSDirection direction, this->directions) {
			int16_t num_frames = direction.frames.length();
			writeFile(outFile, &num_frames, sizeof(num_frames));
			memset(&direction.reserved[0], 0, sizeof(direction.reserved));
			writeFile(outFile, direction.reserved, sizeof(direction.reserved));

			uint16_t str_length = direction.name.length() + 1;
			const uint16_t str_length_const = str_length; // because MSVC loves to be cantankerous with non-constant array indices
			char* direction_name = (char*)direction.name.toStdString().c_str();

			writeFile(outFile, &str_length, sizeof(str_length));
			writeFile(outFile, direction_name, str_length);

			foreach (SSFrame frame, direction.frames) {
				writeFile(outFile, &frame.imageIndex, sizeof(frame.imageIndex));
				writeFile(outFile, &frame.delay, sizeof(frame.delay));
				memset(&frame.reserved[0], 0, sizeof(frame.reserved));
				writeFile(outFile, &frame.reserved, sizeof(frame.reserved));
			}
		}
		break;
	}
	default:
		//this shouldn't technically be possible
		errorBox("Invalid spriteset version: " + QString::number(this->header.version));
		outFile->close();
		return false;
		break;
	}
	return true;
}

void Spriteset::debugDump() {
	qDebug().nospace() <<
		"Spriteset information for \"" << this->filename << "\"\n" <<
		"Version: " << this->header.version << "\n" <<
		"No. images: " << this->header.num_images << "\n" <<
		"Frame width: " << this->header.frame_width << "\n" <<
		"Frame height: " << this->header.frame_height << "\n" <<
		"No. directions: " << this->header.num_directions << "\n" <<
		"Base Upper left: {" << this->header.base_x1 << "," << this->header.base_y1 << "}\n" <<
		"Base Lower right: {" << this->header.base_x2 << "," << this->header.base_y2 << "}\n";
	QString dumpName = "";
	for(int d = 0; d < this->directions.length(); d++) {
		dumpName += this->directions.at(d).name;
		if(d < this->directions.length() - 1) dumpName += ",";
	}
	qDebug().nospace() << dumpName;
	for(int f = 0; f < this->images.length(); f++) {
		this->images.at(f).save("dump_sprite" + QString::number(f) + ".png","PNG");
	}
}

void Spriteset::addDirection(QString name, int numFrames) {
	QList<SSFrame> frames;
	for(int f = 0; f < numFrames; f++) {
		SSFrame frame = SSFrame();
		frame.delay = 8;
		frame.imageIndex = 0;
		frames.append(frame);
	}
	this->addDirection(name, frames);
}

void Spriteset::addDirection(QString name, QList<Spriteset::SSFrame> frames) {
	SSDirection newDirection = SSDirection();
	newDirection.name = (char*)name.toStdString().c_str();
	newDirection.frames = frames;
	this->directions.append(newDirection);
}

void Spriteset::addDirection(QString name, Spriteset::SSFrame frame) {
	QList<SSFrame> frames;
	frames.append(frame);
	this->addDirection(name, frames);
}
