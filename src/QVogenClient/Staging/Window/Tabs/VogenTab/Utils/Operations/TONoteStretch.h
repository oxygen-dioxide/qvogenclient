#ifndef TONOTESTRETCH_H
#define TONOTESTRETCH_H

#include "TBaseOperation.h"

#include <QList>

class TONoteStretch : public TBaseOperation {
public:
    TONoteStretch();
    ~TONoteStretch();

public:
    struct StretchData {
        quint64 id;
        int hStretch;
    };

    QList<StretchData> stretches;
};

#endif // TONOTESTRETCH_H
