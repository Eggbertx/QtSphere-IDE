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
		void loadProject(QSIProject* project);
		void refreshGameList();
		void setGameInfoText(QString name, QString author, QString resolution, QString path, QString description);

	private slots:
		void onProjectIconsCustomContextMenuRequested(const QPoint &pos);
		void onProjectIconsItemActivated(QListWidgetItem *item);
		void onProjectIconsItemSelectionChanged();
		void onStartGame();

	signals:
		void projectLoaded(QSIProject* project);
		void gameStarted(QString gameDir);

	private:
		static bool gameSortHelper(QSIProject* a, QSIProject* b);
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
