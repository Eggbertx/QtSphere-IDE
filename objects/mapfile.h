#ifndef MAPFILE_H
#define MAPFILE_H

#include <QObject>
#include "objects/spherefile.h"

class MapFile : public SphereFile {
    Q_OBJECT

    public:
		MapFile(QWidget *parent = 0);
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
