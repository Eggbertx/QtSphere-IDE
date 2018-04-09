#ifndef SPRITESET_H
#define SPRITESET_H

#include <QWidget>

class Spriteset : public QWidget {
	Q_OBJECT
public:
	explicit Spriteset(QWidget *parent = nullptr);
	~Spriteset();
	bool open(char* filename);
	void debugDump();
	int frameWidth();
	int frameHeight();

protected:
	void paintEvent(QPaintEvent* e);

private:
	QList<QImage> images;
	char* filename;
	const QPixmap alphabg = QPixmap(":/icons/transparency-bg.png");
	typedef struct rss_header {
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

	typedef struct rss_frame {
		uint16_t imageIndex;
		uint16_t delay;
		uint8_t reserved[4];
	}rss_frame;

	typedef struct rss_direction {
		char* name;
		QList<QImage*> frameImages;
		QList<rss_frame> frames;
	}rss_direction;
	QList<rss_direction> directions;

signals:

public slots:

};

#endif // SPRITESET_H
