#ifndef TDIGITALEVENT_H
#define TDIGITALEVENT_H

#include "Types/Events.h"

class TDigitalEvent : public QEventImpl::SceneActionEvent {
public:
    TDigitalEvent(int type);
    ~TDigitalEvent();

    inline int dType() const {
        return dt;
    }

    enum DigitalType {
        Transpose,
        Tempo,
        TimeSig,
    };

    int digit;
    double digitF;

protected:
    int dt;
};

#endif // TDIGITALEVENT_H
