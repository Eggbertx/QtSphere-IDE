#ifndef QSIFILE_H
#define QSIFILE_H
/*
 * QSIFile is an abstract object to be inherited by all of Sphere/miniSphere's filetypes:
 *
 *
 */
#include <QObject>
#include <QFile>
#include <QTextEdit>

class QSIFile : public QFile {
    Q_OBJECT
public:
    //explicit QSIFile(QString fn, QTextEdit *parentWidget);
    explicit QSIFile(QFile *file = nullptr);
    explicit QSIFile(QString fn);

    QString fileExtension();
    QString* fileName();
    QString getText();
    void setText(QString txt);
    QString getBaseName();
signals:

public slots:

private:
    QString fileText;
};

#endif // QSIFILE_H
