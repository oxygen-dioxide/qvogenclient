#ifndef TOSINGERCHANGE_H
#define TOSINGERCHANGE_H

#include "TBaseOperation.h"

class TOSingerChange : public TBaseOperation {
public:
    TOSingerChange();
    ~TOSingerChange();

public:
    QString singerId;
    QString oldSingerId;

    QString rom;
    QString oldRom;

    quint64 gid;
};

#endif // TOSINGERCHANGE_H
