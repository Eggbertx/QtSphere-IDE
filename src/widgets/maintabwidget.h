#ifndef MAINTABWIDGET_H
#define MAINTABWIDGET_H

#include <QTableWidget>

#include "widgets/maintabbar.h"

class MainTabWidget : public QTabWidget {
public:
	MainTabWidget(QWidget* parent = nullptr);
	~MainTabWidget();

protected:
	MainTabBar* m_fileTabBar;
};

#endif // MAINTABWIDGET_H
