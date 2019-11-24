#ifndef SPHEREFONT_H
#define SPHEREFONT_H

#include "formats/spherefile.h"

class SphereFont : public SphereFile {
	public:
		explicit SphereFont(QObject *parent = nullptr);
		bool open(QString filename) override;
		bool save(QString filename) override;
		static SphereFont* fromSystemFont(QWidget* parent = nullptr);
		QImage getTextImage(QString text = nullptr);
		void setCharImage(int c, QImage image);
		inline int numChars() {
			return m_header.num_chars;
		}

	private:
		#pragma pack(push, 1)
		typedef struct header_struct {
			uint8_t signature[4];
			uint16_t version;
			uint16_t num_chars;
			uint8_t reserved[248];
		} rfn_header;
		#pragma pack(pop)
        rfn_header m_header;
		QList<QImage> m_charImages;
        #pragma pack(push, 1)
        typedef struct frame_struct {
            uint16_t width;
            uint16_t height;
            uint8_t reserved[28];
         } rfn_frame;
        #pragma pack(pop)
};

#endif // SPHEREFONT_H
