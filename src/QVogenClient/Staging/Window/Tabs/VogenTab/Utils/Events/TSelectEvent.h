#ifndef TSELECTEVENT_H
#define TSELECTEVENT_H

#include "../Operations/TBaseOperation.h"
#include "Types/Events.h"

class TSelectEvent : public QEventImpl::PianoRollChangeEvent {
public:
    TSelectEvent();
    ~TSelectEvent();

public:
    void dispatch(QObject *current);
};

#endif // TSELECTEVENT_H
