#ifndef MAPFILE_H
#define MAPFILE_H

#include <QObject>
#include <QGraphicsView>

#include "formats/spherefile.h"
#include "formats/tileset.h"

class MapFile : public SphereFile {
	Q_OBJECT
	Q_ENUMS(ScriptType)
	Q_ENUMS(EntityDirection)
	Q_ENUMS(TiledMapFormat)
	public:
		MapFile(QObject *parent = nullptr);
		~MapFile() override;
		void newMap();
		bool open(QString filename, QFile::OpenMode flags = QIODevice::ReadOnly) override;
		bool openTiledMap(QString filename = "");
		bool save(QString filename) override;
		bool mapOrigin();
		void setParallax(bool is_parallax, float mult_x, float mult_y, float scrollspeed_x, float scrollspeed_y);
		QRect* largestLayerRect();
		QSize getTileSize();
		Tileset* getTileset();
		enum ScriptType { Entry, Exit, LeaveNorth, LeaveEast, LeaveSouth, LeaveWest };
		enum EntityDirection { North, Northeast, East, Southeast, South, Southwest, West, Northwest };
		enum TiledMapFormat { TiledUnknownFormat = -1, TiledTMX, TiledJSON };

		QString getScript(ScriptType type);
		void setScript(ScriptType type, QString text);

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
		}layer;

		QList<layer> getLayers();
		layer* getLayer(int index);
		int removeLayer(int index);
		void resizeAllLayers(int width, int height);
		void resizeAllLayers(QSize size);
		int numLayers();
		QString getLayerName(int layer);
		void setLayerName(int layer, QString name);
		bool isLayerVisible(int layer);
		void setLayerVisible(int layer, bool visible);

		int getTileIndex(int l, int x, int y);
		QList<entity> getEntities(int layer = -1);
		entity* getEntity(int index);

	private:
		void reset();
		rmp_header m_header;
		Tileset* m_tileset;
		QString m_tilesetFilename;
		QString m_musicFilename;
		QString m_scriptFilename;

		// map script files, JavaScript source not filename
		QString m_entryScript;
		QString m_exitScript;
		QString m_northScript;
		QString m_eastScript;
		QString m_southScript;
		QString m_westScript;

		QList<layer> m_layers;
		QList<segment_struct> m_segments;
		QList<entity> m_entities;
		QList<zone> m_zones;
};

#endif
