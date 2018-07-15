#include <QDebug>
#include <QImage>
#include <QPoint>

#include "mainwindow.h"
#include "spriteset.h"
#include "util.h"
#include "objects/spherefile.h"

Spriteset::Spriteset(QObject *parent) : SphereFile(parent) {}

Spriteset::~Spriteset() {}

void Spriteset::createNew() {
	this->filename = "Untitled.rss";
}

bool Spriteset::open(QString filename) {
	int file_position = 0;
	this->filename = (char*)filename.toStdString().c_str();
	readFile(filename, &this->header, sizeof(header), 0);
	file_position += sizeof(header);
	if (memcmp(header.signature, ".rss", 4) != 0) {
		errorBox("Error: " + QString(filename) + " is not a valid spriteset!");
		return false;
	}

	switch(header.version) {
	case 1:
	case 2:
		errorBox("v1 and v2 spritesets are not currently supported.");
		return false;
		break;
	case 3: {
		this->images = QList<QImage>();
		for(int i = 0; i < header.num_images; i++) {
			int num_img_bytes = header.frame_width*header.frame_height*4;
			unsigned char image_bytes[num_img_bytes];
			readFile(filename, &image_bytes, num_img_bytes, file_position);
			file_position += num_img_bytes;
			this->images.append(QImage(
				image_bytes,
				header.frame_width, header.frame_height,
				QImage::Format_RGBA8888
			).copy(0,0,header.frame_width,header.frame_height));
		}

		this->directions = QList<rss_direction>();
		for(int d = 0; d < header.num_directions; d++) {
			rss_direction direction;
			uint16_t num_frames;
			readFile(filename, &num_frames, sizeof(num_frames), file_position);
			file_position += sizeof(num_frames) + 6; // 6 reserved bytes

			uint16_t str_length;
			readFile(filename, &str_length, sizeof(str_length), file_position);
			file_position += sizeof(str_length);

			char direction_name[str_length];
			readFile(filename, direction_name, str_length, file_position);
			direction.name = direction_name;
			file_position += str_length;

			direction.frames = QList<rss_frame>();
			for(int f = 0; f < num_frames;f++) {
				rss_frame frame;
				readFile(filename, &frame, sizeof(frame), file_position);
				file_position += sizeof(frame);
			}
			this->directions.append(direction);
		}
		break;
	}
	default:
		errorBox("Invalid spriteset version: " + QString::number(header.version));
		return false;
		break;
	}
	return true;
}

bool Spriteset::save() {
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

QList<Spriteset::rss_direction> Spriteset::getDirections() {
	return this->directions;
}
