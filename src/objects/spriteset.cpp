#include <fstream>
#include <stdio.h>
#include <QByteArray>
#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QPoint>
#include <QPaintEvent>
#include "mainwindow.h"
#include "spriteset.h"
#include "util.h"
#include "objects/spherefile.h"

Spriteset::Spriteset(QWidget *parent) : QWidget(parent) {}
Spriteset::~Spriteset() {}

bool Spriteset::open(char* filename) {
	int file_position = 0;
	this->filename = filename;
	file_read(filename, &this->header, sizeof(header), 0);
	file_position  += sizeof(header);
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
			file_read(filename, &image_bytes, num_img_bytes, file_position);
			file_position += num_img_bytes;
			this->images.append(QImage(
				(unsigned char*)image_bytes,
				header.frame_width, header.frame_height,
				QImage::Format_RGBA8888
			).copy(0,0,header.frame_width,header.frame_height));
		}

		this->directions = QList<rss_direction>();
		for(int d = 0; d < header.num_directions; d++) {
			rss_direction direction;
			uint16_t num_frames;
			file_read(filename, &num_frames, sizeof(num_frames), file_position);
			file_position += sizeof(num_frames) + 6; // 6 reserved bytes

			uint16_t str_length;
			file_read(filename, &str_length, sizeof(str_length), file_position);
			file_position += sizeof(str_length);

			char direction_name[str_length];
			file_read(filename, direction_name, str_length, file_position);
			direction.name = direction_name;
			file_position += str_length;
			qDebug() << direction.name;
			direction.frames = QList<rss_frame>();
			for(int f = 0; f < num_frames;f++) {
				rss_frame frame;
				file_read(filename, &frame, sizeof(frame), file_position);
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
	for(int f = 0; f < this->images.length(); f++) {
		this->images.at(f).save("dump_sprite" + QString::number(f) + ".png","PNG");
	}
}

void Spriteset::paintEvent(QPaintEvent *e) {
	if(e != nullptr) e->ignore();
	QPainter painter(this);

	int all_width = this->images.length()*(this->header.frame_width+4);
	int all_height = (this->header.frame_height+4);
	const QRect all_rect(0,0,all_width,all_height);
	painter.setBrush(Qt::black);
	painter.setPen(Qt::black);
	painter.drawRect(this->rect());
	painter.drawTiledPixmap(all_rect, this->alphabg);

	for(int i = 0; i < this->images.length(); i++) {
		QImage img = this->images.at(i);
		painter.drawImage(QPoint(i * img.width()+4,0),img);
	}
}
