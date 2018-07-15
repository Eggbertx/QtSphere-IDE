#ifndef SPRITESET_H
#define SPRITESET_H

#include <QWidget>
#include "objects/spherefile.h"

class Spriteset : public SphereFile {
	Q_OBJECT
	public:
		explicit Spriteset(QObject *parent = nullptr);
		~Spriteset();
		static struct rss_frame {
			uint16_t imageIndex;
			uint16_t delay;
			uint8_t reserved[4];
		}frame_struct;

		static struct rss_direction {
			char* name;
			QList<rss_frame> frames;
		}direction_struct;

		void createNew() override;
		bool open(QString filename);
		bool save();
		void debugDump();
		int frameWidth();
		int frameHeight();
		QList<rss_direction> getDirections();
		QRect getBaseRect();
		void addDirection(QString name, QList<rss_frame> frames);

		QList<QImage> images;


	private:
		QString filename;
		typedef struct header_struct {
			uint8_t signature[4];	// Must be ".rss"
			uint16_t version;		// Must be 1 or 2, or 3
			uint16_t num_images;	// only valid if version = 1 or version = 3
			uint16_t frame_width;
			uint16_t frame_height;
			uint16_t num_directions;// only valid if version = 2 or version = 3
			uint16_t base_x1;
			uint16_t base_y1;
			uint16_t base_x2;
			uint16_t base_y2;
			uint8_t reserved[106];
		}rss_header;
		rss_header header;
		QList<rss_direction> directions;
};

#endif // SPRITESET_H
