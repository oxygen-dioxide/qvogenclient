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

    struct GroupData {
        quint64 id;
        QString name;
        QString singer;
        QString rom;
    };

    QList<quint64> ids;

    GroupData group;
    GroupData oldGroup;

protected:
    int gc;
};

#endif // TOGROUPCHANGE_H
