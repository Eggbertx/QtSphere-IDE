#ifndef TILESET_H
#define TILESET_H

#include <QObject>
#include "spherefile.h"

class Tileset : public SphereFile {
	Q_OBJECT
	public:
		explicit Tileset(QObject *parent = nullptr);

		bool open(QString filename) override;
		bool save(QString filename) override;

		int getDelay(int index);
		void setDelay(int index);
		QImage getImage(int index);

		#pragma pack(push, 1)
		typedef struct rts_header {
			char		signature[4];
			uint16_t	version; // must be 1
			uint16_t	num_tiles;
			uint16_t	tile_width;
			uint16_t	tile_height;
			uint16_t	color_depth; // must be 32
			uint8_t		compression;
			uint8_t		has_obstruction;
			uint8_t		reserved[240];
		}rts_header;

		typedef struct tile_info {
			uint8_t		obsolete_1;
			uint8_t		animated;
			uint16_t	next_tile;
			uint16_t	delay;
			uint8_t		obsolete_2;
			uint8_t		block_type; // 0 = no obstruction, 1 = old obstruction, 2 = new obstruction
			uint16_t	num_segments;
			uint8_t		reserved[22];
		}tile_info;
		#pragma pack(pop)
		rts_header header;

		typedef struct tile {
			int index;
			QImage image;
			tile_info info;
		}tile;
		QList<QImage> getTileImages();
		QList<tile> getTiles();

	signals:

	public slots:

	private:
		QList<tile> tiles;
};

#endif // TILESET_H
