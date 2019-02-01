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
		explicit StartPage(QWidget *parent = nullptr);
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
		QString m_baseInfoHTML;
		QList<QSIProject*> m_gameList;
		QSIProject* m_currentProject;

		// Right click menu and options
		QMenu* m_rightClickMenu;
		QAction* m_startGameAction;
		QAction* m_loadProjectAction;
		QAction* m_openProjectDirAction;
};

#endif // STARTPAGE_H
