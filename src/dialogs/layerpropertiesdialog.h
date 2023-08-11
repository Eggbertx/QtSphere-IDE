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
		void onParallaxChkStateChanged(int arg1);
		void onHorizontalParallaxSliderValueChanged(int value);
		void onVerticalParallaxSliderValueChanged(int value);
		void onHorizontalAsSliderValueChanged(int value);
		void onVerticalAsSliderValueChanged(int value);

	private:
		Ui::LayerPropertiesDialog *ui;
		void updateSliderIndicator(QSlider* slider, QLabel* indicator);
};

#endif // LAYERPROPERTIESDIALOG_H
