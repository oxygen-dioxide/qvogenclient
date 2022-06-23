#ifndef TOPERATEEVENT_H
#define TOPERATEEVENT_H

#include "../Operations/TBaseOperation.h"
#include "Types/Events.h"

class TOperateEvent : public QEventImpl::PianoRollChangeEvent {
public:
    TOperateEvent();
    ~TOperateEvent();

public:
    void dispatch(QObject *current);

    TBaseOperation *data() const;
    void setData(TBaseOperation *data);
    TBaseOperation *takeData();

protected:
    TBaseOperation *m_data;
};

#endif // TOPERATEEVENT_H
