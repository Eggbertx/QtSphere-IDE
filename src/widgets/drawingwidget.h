#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include <QWidget>

namespace Ui {
	class DrawingWidget;
}

class DrawingWidget : public QWidget {
	Q_OBJECT

	public:
		explicit DrawingWidget(QWidget *parent = 0);
		~DrawingWidget();

	private:
		Ui::DrawingWidget *ui;
};

#endif // DRAWINGWIDGET_H
