#ifndef SPRITESET_H
#define SPRITESET_H


#include <QFile>
#include "formats/spherefile.h"
#include "dialogs/importoptionsdialog.h"

class Spriteset : public SphereFile {
	Q_OBJECT
	public:
		explicit Spriteset(QObject *parent = nullptr);
		~Spriteset() override;
		static Spriteset* fromImage(QString filename, QSize frameSize, bool removeDuplicates, QColor inColor, QColor outColor, bool* success = nullptr);
		static struct SSFrame {
			uint16_t imageIndex;
			uint16_t delay;
			uint8_t reserved[4];
		}frame_struct;

		static struct SSDirection {
			QString name;
			QList<SSFrame> frames;
			uint8_t reserved[6];
		}direction_struct;

		void createNew() override;
		bool open(QString filename) override;
		bool save(QString filename) override;
		void debugDump();
		int frameWidth();
		int frameHeight();

		QRect getBaseRect();
		int numDirections();
		void addDirection(SSDirection* direction);
		SSDirection* addDirection(QString name, int numFrames);
		SSDirection* addDirection(QString name, QList<SSFrame> frames);
		void addDirection(QString name, SSFrame frame);
		SSDirection* getDirection(int index);
		void setDirectionName(int index, QString name);

		int numImages();
		QImage* getImage(int index);
		void addImage(QImage image, bool mustBeUnique = false);
		QList<QImage> getImages();
		void setImages(QList<QImage> images);
		int removeDuplicateImages();



	private:
		QString m_filename;
		#pragma pack(push, 1)
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
		rss_header m_header;
		#pragma pack(pop)

		QList<SSDirection> m_directions;
		QList<QImage> m_images;

};

#endif // SPRITESET_H
