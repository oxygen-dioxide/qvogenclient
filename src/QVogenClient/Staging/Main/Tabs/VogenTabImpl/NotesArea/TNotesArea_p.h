#ifndef TNOTESAREAPRIVATE_H
#define TNOTESAREAPRIVATE_H

#include "TNotesArea.h"

class TNotesAreaPrivate {
    Q_DECLARE_PUBLIC(TNotesArea)
public:
    TNotesAreaPrivate();
    virtual ~TNotesAreaPrivate();

    void init();

    TNotesArea *q_ptr;
};

#endif // TNOTESAREAPRIVATE_H
