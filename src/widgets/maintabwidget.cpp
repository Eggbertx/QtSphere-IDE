#include "maintabwidget.h"

MainTabWidget::MainTabWidget(QWidget* parent): QTabWidget(parent) {
	m_fileTabBar = new MainTabBar(this);
	setTabBar(m_fileTabBar);
}

MainTabWidget::~MainTabWidget() {
	delete m_fileTabBar;
}
