#ifndef STARTPAGE_H
#define STARTPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QListWidgetItem>

#include "qsiproject.h"

namespace Ui {
	class StartPage;
}

class StartPage : public QWidget {
	Q_OBJECT

	public:
		explicit StartPage(QWidget *parent = 0);
		~StartPage();
		QString getGameInfoText();
		void refreshGameList();
		void setGameInfoText(QString name, QString author, QString resolution, QString path, QString description);

	private slots:
		void on_projectIcons_customContextMenuRequested(const QPoint &pos);
		void on_projectIcons_itemActivated(QListWidgetItem *item);
		void on_projectIcons_itemSelectionChanged();

	private:
		Ui::StartPage *ui;
		QString baseInfoHTML;
		QList<QSIProject*> gameList;
		// Right click menu and options
		QMenu* rightClickMenu;
		QAction* startGameAction;
		QAction* loadProjectAction;
		QAction* openProjectDirAction;
};

#endif // STARTPAGE_H
