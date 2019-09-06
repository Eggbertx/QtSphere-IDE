#include <QBitmap>
#include <QBuffer>
#include <QByteArray>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPoint>

#include "mainwindow.h"
#include "formats/spriteset.h"
#include "util.h"
#include "formats/spherefile.h"
#include "dialogs/importoptionsdialog.h"

Spriteset::Spriteset(QObject *parent) : SphereFile(parent) {
	m_images = QList<QImage>();
	m_directions = QList<SSDirection>();
}

Spriteset::~Spriteset() {
	delete m_file;
}

Spriteset* Spriteset::fromImage(QString filename, QSize frameSize, bool removeDuplicates, QColor inColor, QColor outColor, bool* success) {
	Spriteset* newSS = new Spriteset();
	bool valid = false;

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

	int numFramesX = importImg->width()/frameW;
	int numFramesY = importImg->height()/frameH;

	for(int y = 0; y < numFramesY; y++) {
		for(int x = 0; x < numFramesX; x++) {
			QImage image = importImg->copy(x*frameW,y*frameH,frameW,frameH);
			newSS->addImage(QImage(image), removeDuplicates);
		}
	}

	rss_header header;
	memcpy(header.signature, ".rss", 4);
	header.version = 3;
	header.num_images = newSS->numImages();
	header.frame_width = frameW;
	header.frame_height = frameH;
	header.num_directions = 4;
	header.base_x1 = 0;
	header.base_y1 = 0;
	header.base_x2 = 0;
	header.base_y2 = 0;
	newSS->m_header = header;

	newSS->addDirection("north", 3);
	newSS->addDirection("east", 3);
	newSS->addDirection("south", 3);
	newSS->addDirection("west", 3);
	if(success) *success = true;
	return newSS;
}


void Spriteset::createNew() {
	m_filename = "Untitled.rss";
}

bool Spriteset::open(QString filename) {
	m_filename = filename;
	m_file = new QFile(m_filename);

	if(!m_file->exists()) {
		errorBox("The spriteset " + filename + " doesn't exist!");
		return false;
	}
	if(!m_file->open(QIODevice::ReadOnly)) {
		errorBox("Failed loading spriteset " + filename);
		m_file->close();
		return false;
	}
	readFile(m_file, &m_header, sizeof(m_header));
	if (memcmp(m_header.signature, ".rss", 4) != 0) {
		errorBox("Error: " + QString(filename) + " is not a valid spriteset!");
		m_file->close();
		return false;
	}

	switch(m_header.version) {
	case 1:
	case 2:
		errorBox("v1 and v2 spritesets are not currently supported.");
		m_file->close();
		return false;
	case 3: {
		for(int i = 0; i < m_header.num_images; i++) {
			int num_img_bytes = m_header.frame_width*m_header.frame_height*4;
			unsigned char* image_bytes = new unsigned char[num_img_bytes];

			readFile(m_file, image_bytes, num_img_bytes);

			m_images.append(QImage(image_bytes,
				m_header.frame_width, m_header.frame_height,
				QImage::Format_RGBA8888
			).copy(0,0,m_header.frame_width,m_header.frame_height));
		}

		for(int d = 0; d < m_header.num_directions; d++) {
			SSDirection direction = SSDirection();
			uint16_t num_frames;
			readFile(m_file, &num_frames, sizeof(num_frames));

			//QFile::skip() is apparently a relatively new method
			char* dump6 = (char*)malloc(6);
			readFile(m_file, dump6, 6);
			free(dump6);

			uint16_t str_length;
			readFile(m_file, &str_length, sizeof(str_length));

			const uint16_t str_length_const = str_length; // because MSVC loves to be cantankerous with non-constant array indices
			char* direction_name = new char[str_length_const];
			readFile(m_file, direction_name, str_length);
			direction.name = QString(direction_name);

			direction.frames = QList<SSFrame>();
			for(int f = 0; f < num_frames;f++) {
				SSFrame frame = SSFrame();
				readFile(m_file, &frame, sizeof(frame));
				direction.frames.append(frame);
			}

			m_directions.append(direction);
		}
		break;
	}
	default:
		errorBox("Invalid spriteset version: " + QString::number(m_header.version));
		m_file->close();
		return false;
	}
	m_file->close();
	delete m_file;
	return true;
}

bool Spriteset::save(QString filename) {
	QFile* outFile = new QFile(filename);
	memset(&m_header.reserved[0], 0, sizeof(m_header.reserved));

	writeFile(outFile, &m_header, sizeof(m_header));

	switch(m_header.version) {
	case 1:
	case 2:
		errorBox("v1 and v2 spritesets are not currently supported.");
		outFile->close();
		return false;
	case 3: {
		foreach(QImage image, m_images) {
			QByteArray* bytes = imageBytes(&image);
			writeFile(outFile, bytes->data(), bytes->length());
		}

		foreach(SSDirection direction, m_directions) {
			int16_t num_frames = direction.frames.length();
			writeFile(outFile, &num_frames, sizeof(num_frames));
			memset(&direction.reserved[0], 0, sizeof(direction.reserved));
			writeFile(outFile, direction.reserved, sizeof(direction.reserved));

			uint16_t str_length = direction.name.length() + 1;

			writeFile(outFile, &str_length, sizeof(str_length));
            writeFile(outFile, toString(direction.name), str_length);

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
		errorBox("Invalid spriteset version: " + QString::number(m_header.version));
		outFile->close();
		return false;
	}
	return true;
}

void Spriteset::debugDump() {
	qDebug("Spriteset information for \"%s\":\n"
		"Version: %d\n"
		"No. images: %d\n"
		"Frame size: %dx%d\n"
		"No. directions: %d\n"
		"Base upper left: {%d,%d}\n"
		"Base lower right: {%d,%d}",
        toString(m_filename), m_header.version, m_header.num_images, m_header.frame_width, m_header.frame_height,
		m_header.num_directions, m_header.base_x1, m_header.base_y1, m_header.base_x2, m_header.base_y2);

	QString dumpName = "";
	for(int d = 0; d < m_directions.length(); d++) {
		dumpName += m_directions.at(d).name;
		if(d < m_directions.length() - 1) dumpName += ",";
	}

	for(int f = 0; f < m_images.length(); f++) {
		m_images.at(f).save("dump_sprite" + QString::number(f) + ".png","PNG");
	}
}

int Spriteset::numDirections() {
	return m_header.num_directions;
}

void Spriteset::addDirection(Spriteset::SSDirection* direction) {
	m_directions.append(*direction);
	m_header.num_directions++;
}

Spriteset::SSDirection* Spriteset::addDirection(QString name, int numFrames) {
	QList<SSFrame> frames;
	for(int f = 0; f < numFrames; f++) {
		SSFrame frame = SSFrame();
		frame.delay = 8;
		frame.imageIndex = 0;
		frames.append(frame);
	}
	return addDirection(name, frames);
}

Spriteset::SSDirection* Spriteset::addDirection(QString name, QList<Spriteset::SSFrame> frames) {
	SSDirection newDirection = SSDirection();
    newDirection.name = toString(name);

	newDirection.frames = frames;
	m_directions.append(newDirection);
	m_header.num_directions++;
	return &m_directions.last();
}

void Spriteset::addDirection(QString name, Spriteset::SSFrame frame) {
	QList<SSFrame> frames;
	frames.append(frame);
	addDirection(name, frames);
}

Spriteset::SSDirection* Spriteset::getDirection(int index) {
	if(index > numDirections()) return nullptr;
	return &m_directions[index];
}

void Spriteset::setDirectionName(int index, QString name) {
	if(index > numDirections()) return;
	m_directions[index].name = name;
}

int Spriteset::numImages() {
	return m_header.num_images;
}

QImage* Spriteset::getImage(int index) {
	if(m_images.size() > index) return &m_images[index];
	return nullptr;
}

void Spriteset::addImage(QImage image, bool mustBeUnique) {
	if((mustBeUnique && !m_images.contains(image)) || !mustBeUnique) {
		m_images.append(image);
	}
}

QList<QImage> Spriteset::getImages() {
	return m_images;
}

void Spriteset::setImages(QList<QImage> images) {
	m_images = images;
}

int Spriteset::removeDuplicateImages() {
	return 0;
}
