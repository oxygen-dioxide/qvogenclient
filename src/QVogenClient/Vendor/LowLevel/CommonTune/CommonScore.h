#ifndef COMMONSCORE_H
#define COMMONSCORE_H

#include <QList>
#include <QPoint>

#include "CommonNote.h"

class CommonScore {
public:
    CommonScore();
    ~CommonScore();

public:
    // Track Name
    QString name;

    // Global Tempo
    double tempo;

    // Time Signature
    QPoint beat;

    // Note Sequence
    struct Track {
        QString name;
        QList<CommonNote> notes;
    };

    QList<Track> tracks;
};

#endif // COMMONSCORE_H
