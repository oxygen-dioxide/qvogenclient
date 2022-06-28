#ifndef TDIGITTIMESIGEVENT_H
#define TDIGITTIMESIGEVENT_H

#include "TDigitalEvent.h"

class TDigitTimeSigEvent : public TDigitalEvent {
public:
    TDigitTimeSigEvent();
    ~TDigitTimeSigEvent();

public:
    int at;
    int b;
};

#endif // TDIGITTIMESIGEVENT_H
