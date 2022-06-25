#ifndef TSSQCURSORMODEEVENT_H
#define TSSQCURSORMODEEVENT_H

#include "Types/Events.h"

class TSSQCursorModeEvent : public QEventImpl::SceneStateQueryEvent {
public:
    TSSQCursorModeEvent();
    ~TSSQCursorModeEvent();

public:
    int mode;
};

#endif // TSSQCURSORMODEEVENT_H
