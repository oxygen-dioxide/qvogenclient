#ifndef TONOTEMOVE_H
#define TONOTEMOVE_H

#include "TBaseOperation.h"

#include <QList>

class TONoteMove : public TBaseOperation {
public:
    TONoteMove();
    ~TONoteMove();

public:
    struct MoveData {
        quint64 id;
        int hMove;
        int vMove;
    };

    QList<MoveData> moves;
};

#endif // TONOTEMOVE_H
