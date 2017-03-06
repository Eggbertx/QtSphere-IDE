#ifndef QSIFILE_H
#define QSIFILE_H

#include <QObject>
#include <QFile>
#include <QTextEdit>

// an abstract type that all Sphere types (Map, Tileset, Spriteset, etc) will inherit
class QSIFile : public QFile {
	Q_OBJECT
	public:
        explicit QSIFile(QString fn, QTextEdit *parentWidget);
		explicit QSIFile(QFile file);
		enum FileTypeEnum { TextFile = 0, JavaScript = 1, TypeScript = 2, Map = 4, WindowStyle = 8, SpriteSet = 16, Font = 32, Image = 64, Sound = 128 };
		Q_DECLARE_FLAGS(FileTypes, FileTypeEnum)
		int fileType; // one of the above
		QString text; // used for plain text files and scripts
        QString baseName();
		bool checkIfModified();
		bool save();
		bool saveAs();
        bool isModified(QTextEdit *editor);
		bool isLoaded(); // so we don't repeatedly initialize the tab
		void setLoaded();
        QTextEdit* textEditor();
        void attachToTextEditor(QTextEdit *editor);
        QFile *file();

	private:
        QTextEdit *textEditWidget;
		bool loadedStatus;
		void processFile();
        QFile* _file;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSIFile::FileTypes)

#endif // QSIFILE_H
