#ifndef TSETPLAYHEADEVENT_H
#define TSETPLAYHEADEVENT_H

#include "../TPianoRollEvent.h"

class TSetPlayheadEvent : public TPianoRollEvent {
public:
    TSetPlayheadEvent();
    ~TSetPlayheadEvent();

public:
    int x;
};

#endif // TSETPLAYHEADEVENT_H
