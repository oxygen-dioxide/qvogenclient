#ifndef TDIGITALEVENT_H
#define TDIGITALEVENT_H

#include "Types/Events.h"

class TDigitalEvent : public QEventImpl::SceneActionRequestEvent {
public:
    TDigitalEvent(int type);
    ~TDigitalEvent();

    inline int dType() const {
        return dt;
    }

    enum DigitalType {
        Transpose,
    };

    int digit;

protected:
    int dt;
};

#endif // TDIGITALEVENT_H
