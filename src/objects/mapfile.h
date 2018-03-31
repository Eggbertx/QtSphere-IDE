#ifndef MAPFILE_H
#define MAPFILE_H

#include <QObject>
#include <QGraphicsView>

class MapFile : public QGraphicsView {
	Q_OBJECT

	public:
		MapFile(QWidget *parent = nullptr);
		void newMap();
		bool loadMap(QString filename);
		bool saveMap(QString filename);
		QString getMapName();
		void setMapName(QString name);
		bool mapOrigin();
		void setParallax(
			bool is_parallax,
			float multiplier_x, float multiplier_y,
			float scrollspeed_x, float floatscrollspeed_
		);
		struct parallax_info getParallaxInfo();

	signals:

	public slots:

	private:
		QString mapName;
		QGraphicsView mapWidget;

		// The following structs were taken from miniSphere's code
		struct fileHeader {
			char    signature[4];
			int16_t version;
			uint8_t type;
			int8_t  num_layers;
			uint8_t reserved_1;
			int16_t num_entities;
			int16_t start_x;
			int16_t start_y;
			int8_t  start_layer;
			int8_t  start_direction;
			int16_t num_strings;
			int16_t num_zones;
			uint8_t repeat_map;
			uint8_t reserved[234];
		};

		struct entityHeader {
			uint16_t x;
			uint16_t y;
			uint16_t z;
			uint16_t type;
			uint8_t reserved[8];
		};

		struct layerHeader {
			int16_t  width;
			int16_t  height;
			uint16_t flags;
			float    parallax_x;
			float    parallax_y;
			float    scrolling_x;
			float    scrolling_y;
			int32_t  num_segments;
			uint8_t  is_reflective;
			uint8_t reserved[3];
		};

		struct zoneHeader {
			uint16_t x1;
			uint16_t y1;
			uint16_t x2;
			uint16_t y2;
			uint16_t layer;
			uint16_t interval;
			uint8_t reserved[4];
		};

		struct parallaxInfo {
			bool isParallax;
			float parallaxMultiplier_x;
			float parallaxMultiplier_y;
			float parallaxScrollSpeed_x;
			float parallaxScrollSpeed_y;
		};
};

#endif
