#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include <QWidget>
#include <QMenu>
#include <QColor>
#include <QList>
#include "widgets/spriteset/spriteseteditor.h"

class PaletteWidget : public QWidget {
	Q_OBJECT

public:
	explicit PaletteWidget(SpritesetEditor *parent = nullptr);
	~PaletteWidget();
		void changePalette(QList<QColor> palette);
		void importPalette(QString path);
		void exportPalette(QString path);

		int getSelectedIndex();
		int getSquareSize();

	protected:
		bool eventFilter(QObject* object, QEvent* event);

	private:
		QMenu* m_rightClickMenu;
		QMenu* m_fileMenu;
		QMenu* m_defaultMenu;
		QPoint m_mousePos;
		int m_selectedIndex;
		int m_squareSize = 10;
		QList<QColor> m_paletteColors;
};

#endif // PALETTEWIDGET_H
