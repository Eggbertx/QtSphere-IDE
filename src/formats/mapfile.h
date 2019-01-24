#ifndef MAPFILE_H
#define MAPFILE_H

#include <QObject>
#include <QGraphicsView>

#include "formats/spherefile.h"
#include "formats/tileset.h"

class MapFile : public SphereFile {
	Q_OBJECT

	public:
		MapFile(QObject *parent = nullptr);
		void newMap();
		bool open(QString filename) override;
		bool save(QString filename) override;
		bool mapOrigin();
		void setParallax(bool is_parallax, float mult_x, float mult_y, float scrollspeed_x, float scrollspeed_y);
		QRect *largestLayerRect();
		QSize tileSize();
		QSize mapSize();

		QString tilesetFilename;
		QString musicFilename;
		QString scriptFilename;
		QString entryScript; // JavaScript source, not filename
		QString exitScript;
		QString northScript;
		QString eastScript;
		QString southScript;
		QString westScript;

		#pragma pack(push, 1)
		typedef struct rmp_header {
			char		signature[4]; // must be ".rmp"
			uint16_t	version; // must be 1
			uint8_t		type; // obsolete, ignored
			uint8_t		num_layers;
			uint8_t		reserved_1;
			uint16_t	num_entities;
			uint16_t	start_x;
			uint16_t	start_y;
			uint8_t		start_layer;
			uint8_t		start_direction;
			uint16_t	num_strings; // must be 3, 5, or 9
			uint16_t	num_zones;
			uint8_t		repeating;
			uint8_t		reserved[234];
		}rmp_header;
		rmp_header header;

		typedef struct layer_header {
			int16_t		width;
			int16_t		height;
			uint16_t	flags;
			float		parallax_x;
			float		parallax_y;
			float		scrolling_x;
			float		scrolling_y;
			uint32_t	num_segments;
			uint8_t		reflective;
			uint8_t		reserved[3];
		}layer_header;

		typedef struct segment_struct {
			int		layer;
			float	x1;
			float	y1;
			float	x2;
			float	y2;
		}obs_segment;

		typedef struct entity_struct {
			uint16_t	x;
			uint16_t	y;
			uint16_t	layer;
			uint16_t	type;
			QString		name; // Person (type == 1)
			QString		spriteset; // Person (type == 1)

			uint16_t	num_scripts; // should be 5
			QString		on_create;
			QString		on_destroy;
			QString		on_activate_touch;
			QString		on_activate_talk;
			QString		generate_commands;

			QString		trigger_function; // Trigger (type == 2)
		}entity;

		typedef struct zone_struct {
			uint16_t	x1;
			uint16_t	y1;
			uint16_t	x2;
			uint16_t	y2;
			uint16_t	layer;
			uint16_t	reactivation_delay;
			QString		function;
		}zone;
		#pragma pack(pop)

		typedef struct layer {
			int index;
			QString name;
			layer_header header;
			QList<uint16_t> tiles;
			bool visible; // this is used by MapEditor and isn't saved to/loaded from the file
		}layer;

		QList<layer> getLayers();
		layer* getLayer(int index);
		int getTileIndex(int l, int x, int y);
		Tileset* tileset;

	signals:

	public slots:

	private:
		QList<layer> layers;
		QList<segment_struct> segments;
		QList<entity> entities;
		QList<zone> zones;
};

#endif
