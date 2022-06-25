#ifndef TOTEMPOTIMESIG_H
#define TOTEMPOTIMESIG_H

#include "TBaseOperation.h"

#include <QPair>

class TOTempoTimeSig : public TBaseOperation {
public:
    TOTempoTimeSig();
    ~TOTempoTimeSig();

public:
    struct Data {
        double tempo;
        QPair<int, int> timeSig;
    };

    Data val;
    Data oldVal;
};

#endif // TOTEMPOTIMESIG_H
