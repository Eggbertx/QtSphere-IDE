#ifndef WRAPPEDGRAPHICSVIEW_H
#define WRAPPEDGRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>

class WrappedGraphicsView : public QGraphicsView {
	Q_OBJECT
	public:
		explicit WrappedGraphicsView(QWidget* parent = nullptr);
		void addPixmap(QPixmap pixmap);
		QSize tileSize();
		void setScaleFactor(int factor);
		int scaleFactor();

	protected:
		void resizeEvent(QResizeEvent* resize) override;
		void mouseReleaseEvent(QMouseEvent* event) override;
		int indexAt(QPoint pos);

	signals:
		void indexChanged(int value);

	private:
		int arrangeItems(int width = -1, int height = -1);
		int selectedIndex;
		QSize tSize;
		int scaleMult;
		QGraphicsScene* wScene;
		QList<QPixmap> pixmaps;
};

#endif // WRAPPEDGRAPHICSVIEW_H
