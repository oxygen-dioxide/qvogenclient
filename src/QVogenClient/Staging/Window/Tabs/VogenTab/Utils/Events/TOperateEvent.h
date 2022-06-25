#ifndef TOPERATEEVENT_H
#define TOPERATEEVENT_H

#include "../Operations/TBaseOperation.h"
#include "TPianoRollEvent.h"

class TOperateEvent : public TPianoRollEvent {
public:
    TOperateEvent();
    ~TOperateEvent();

public:
    TBaseOperation *data() const;
    void setData(TBaseOperation *data);
    TBaseOperation *takeData();

protected:
    TBaseOperation *m_data;
};

#endif // TOPERATEEVENT_H
