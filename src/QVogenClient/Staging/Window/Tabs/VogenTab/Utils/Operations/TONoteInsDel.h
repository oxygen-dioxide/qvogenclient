#ifndef TONOTEINSDEL_H
#define TONOTEINSDEL_H

#include "TBaseOperation.h"

#include <QList>

class TONoteInsDel : public TBaseOperation {
public:
    enum Change {
        Create,
        Remove,
    };

    TONoteInsDel(int type);
    ~TONoteInsDel();

public:
    struct NoteData {
        quint64 id;
        int start;
        int length;
        int noteNum;
        QString lyric;
        quint64 gid;
    };

    QList<NoteData> data;

    inline int cType() const {
        return c;
    }

protected:
    int c;
};

#endif // TONOTEINSDEL_H
