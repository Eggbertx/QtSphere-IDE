#ifndef QSITASKLIST_H
#define QSITASKLIST_H

#include <QFile>
#include <QObject>
#include <QTableWidget>

class QSITaskList : public QObject {
	Q_OBJECT
	public:
		explicit QSITaskList(QTableWidget* parent = nullptr);
		void attach(QTableWidget* widget);
		void loadList(QString path);
		void saveList(bool saveAs);
		void addTask(QString str);
		void setTaskStatus(int index, bool status);
		int setTaskString(int index, QString str);
		int removeTask(int index);
	signals:

	private:
		QTableWidget* m_widget = nullptr;
		QFile* m_file = nullptr;
		QList<bool> m_checks;
		QList<QString> m_strings;
};

#endif // QSITASKLIST_H
