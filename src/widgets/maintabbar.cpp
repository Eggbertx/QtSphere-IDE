#include "maintabbar.h"

MainTabBar::MainTabBar(QWidget *parent): QTabBar(parent) {
	setTabsClosable(true);
}

void MainTabBar::mouseReleaseEvent(QMouseEvent* event) {
	if(event->button() == Qt::MidButton) {
		int index = tabAt(event->pos());
		if(index > -1)
			emit tabCloseRequested(index);
	}
	event->accept();
}
