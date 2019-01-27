#ifndef LAYERPROPERTIESDIALOG_H
#define LAYERPROPERTIESDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QSlider>

namespace Ui {
	class LayerPropertiesDialog;
}

class LayerPropertiesDialog : public QDialog {
	Q_OBJECT

	public:
		explicit LayerPropertiesDialog(QWidget *parent = nullptr);
		~LayerPropertiesDialog();
		QString getName();
		void setName(QString name);
		int getWidth();
		int getHeight();
		void setSize(int width, int height);
		int getHorizontalParallax();
		int getVerticalParallax();
		void setParallax(int h, int v);
		bool parallaxEnabled();
		void setParallaxEnabled(bool enabled);
		void setReflectiveEnabled(bool enabled);
		bool reflectiveEnabled();
		int getAutoScrollingWidth();
		int getAutoScrollingHeight();
		void setAutoScrolling(int h, int v);

	private slots:
		void on_parallaxChk_stateChanged(int arg1);
		void on_horizontalParallaxSlider_valueChanged(int value);
		void on_verticalParallaxSlider_valueChanged(int value);
		void on_horizontalAsSlider_valueChanged(int value);
		void on_verticalAsSlider_valueChanged(int value);

	private:
		Ui::LayerPropertiesDialog *ui;
		void updateSliderIndicator(QSlider* slider, QLabel* indicator);
};

#endif // LAYERPROPERTIESDIALOG_H
