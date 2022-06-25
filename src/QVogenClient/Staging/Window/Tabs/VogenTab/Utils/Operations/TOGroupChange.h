#ifndef TOGROUPCHANGE_H
#define TOGROUPCHANGE_H

#include "TBaseOperation.h"

#include <QList>

class TOGroupChange : public TBaseOperation {
public:
    TOGroupChange(int change);
    ~TOGroupChange();

    inline int groupChange() const {
        return gc;
    }

public:
    enum Changes {
        NewAndMove,
        Move,
        MoveAndDel,
    };

    QList<quint64> ids;

    quint64 gid;
    quint64 oldGid;

protected:
    int gc;
};

#endif // TOGROUPCHANGE_H
