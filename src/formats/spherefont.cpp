#include <QBuffer>
#include <QColorDialog>
#include <QFileDialog>
#include <QFontDialog>
#include <QFontMetrics>
#include <QPainter>
#include <QTextEncoder>
#include <QTextDecoder>
#include "util.h"
#include "formats/spherefont.h"

SphereFont::SphereFont(QObject* parent) : SphereFile(parent)  {
	m_charImages.reserve(256);
}

bool SphereFont::open(QString filename) {
	m_file = new QFile(filename);
	if(!m_file->open(QIODevice::ReadOnly)) {
		errorBox("ERROR: Could not read file '" + filename + "': " + m_file->errorString());
		m_file->close();
		return false;
	}
	readFile(m_file, &m_header, sizeof(m_header));

	if(memcmp(m_header.signature, ".rfn", 4) != 0) {
		errorBox("Error: '" + filename + "' is not a valid Sphere font file (invalid signature)!");
		m_file->close();
		return false;
	}

	switch(m_header.version) {
	case 1:
		errorBox("Error: v1 Sphere fonts are not supported yet.");
		m_file->close();
		return false;
	case 2:
		for(int c = 0; c < m_header.num_chars; c++) {
			uint16_t char_width;
			readFile(m_file, &char_width, sizeof(char_width));
			uint16_t char_height;
			readFile(m_file, &char_height, sizeof(char_height));
		}
		break;
	default:
		errorBox("Error: '" + filename + "' is not a valid Sphere font file (invalid version)!");
		m_file->close();
		return false;
	}

	m_file->close();
	return true;
}

bool SphereFont::save(QString filename) {
    //memcpy(m_header.signature, ".rss", 4);

	QByteArray ba;
    QFile* fontFile = new QFile(filename);
    writeFile(fontFile, &m_header, sizeof(m_header));


	for(int i = 0; i < m_header.num_chars; i++) {
        QImage frameImage = m_charImages.at(i).convertToFormat(QImage::Format_RGBA8888);
        rfn_frame frame;
        frame.width = frameImage.width();
        frame.height = frameImage.height();
        writeFile(fontFile, &frame, sizeof(frame));

        QByteArray* bytes = imageBytes(&frameImage);
        char* pixels = bytes->data();
        writeFile(fontFile, bytes->data(), bytes->length());
	}

	return true;
}

SphereFont* SphereFont::fromSystemFont(QWidget* parent) {
	SphereFont* rfn = nullptr;

	bool ok = false;
	QFont baseFont = QFontDialog::getFont(&ok, parent);
	baseFont.setStyleStrategy(QFont::PreferAntialias);
	if(!ok) return rfn; // user cancelled

	QColor fontColor = QColorDialog::getColor(nullptr, parent, "Choose Font Color", QColorDialog::ShowAlphaChannel|QColorDialog::DontUseNativeDialog);
	if(fontColor == nullptr) return rfn; // user cancelled

	rfn = new SphereFont(parent);
    rfn_header header;

    memcpy(header.signature, ".rfn", 4);
    header.version = 2;
    header.num_chars = 256;
    rfn->m_header = header;

	QFontMetrics fm(baseFont);
    QString str = "";
    QSize asciiSize;
	QPainter* painter = new QPainter();

	for(int i = 0; i < 256; i++) {
        ushort ch = i;
        if(i < 32 || i == 127)
            str = "";
        else
            str = QString::fromUtf16(&ch, 1);

		asciiSize = fm.size(Qt::TextSingleLine, str.toStdString().c_str());
		if(asciiSize.width() < 1) asciiSize.setWidth(4);

		QImage canvas(asciiSize, QImage::Format_ARGB32);
		canvas.fill(Qt::transparent);

		if(canvas.isNull()) // this shouldn't happen.
			continue;

		painter->begin(&canvas);
		painter->setPen(fontColor);
		painter->setFont(baseFont);
		painter->drawText(0, 0, asciiSize.width(), asciiSize.height(), Qt::AlignCenter, str.toStdString().c_str());
		painter->end();

        rfn->setCharImage(i, canvas);
	}

	delete painter;

    QString filename = QFileDialog::getSaveFileName(nullptr, "Save script", "","Sphere font (*.rfn;;All files (*)");
    rfn->save(filename);
	return rfn;
}

QImage SphereFont::getTextImage(QString text) {
	int imgW = 0;
	int imgH = 0;
	QString str = "";
	if(text == nullptr) {
		for(int i = 0; i < 256; i++) {
            ushort ch = i;
            if(i < 32 || i == 127)
                str = "";
            else
                str += QString::fromUtf16(&ch, 1);
		}
	} else str = text;
	QList<int> ascii_vals;

	imgH = this->m_charImages.at(0).height();
	foreach(QChar c, text) {
		uint8_t i = c.toLatin1();
		ascii_vals.append(i);
		imgW += m_charImages.at(i).width();
	}
	QImage img = QImage(imgW, imgH, QImage::Format_ARGB32);
	img.fill(Qt::transparent);
	QPainter painter(&img);
	int x = 0;
	foreach(int i, ascii_vals) {
		QImage charImg =  m_charImages.at(i);
		painter.drawImage(x, 0, charImg);
		x += charImg.width();
	}
	return img;
}

void SphereFont::setCharImage(int c, QImage image) {
    m_charImages.insert(c, image.copy(0,0,image.width(),image.height()));
}
