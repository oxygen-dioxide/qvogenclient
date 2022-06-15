#ifndef COMMONSCORE_H
#define COMMONSCORE_H

#include <QList>

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
    QList<CommonNote> notes;
};

#endif // COMMONSCORE_H
