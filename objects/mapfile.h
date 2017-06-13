#ifndef MAPFILE_H
#define MAPFILE_H

#include <QObject>
#include "objects/qsifile.h"

class MapFile : public QSIFile {
    Q_OBJECT

    public:
        MapFile(QSIFile *parent = 0);
        bool running();
        QString getMapName();
        void setMapName(QString name);

        bool mapOrigin();

    signals:

    public slots:

    private:
        QString mapName;
};

#endif
