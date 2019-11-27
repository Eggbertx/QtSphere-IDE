#ifndef MAINTABBAR_H
#define MAINTABBAR_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QTabBar>
#include <QWidget>

class MainTabBar : public QTabBar {
public:
	MainTabBar(QWidget* parent = nullptr);

protected:
	void mouseReleaseEvent(QMouseEvent* event) override;
};

#endif // MAINTABBAR_H
