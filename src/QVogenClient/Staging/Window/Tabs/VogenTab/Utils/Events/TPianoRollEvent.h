#ifndef TPIANOROLLEVENT_H
#define TPIANOROLLEVENT_H

#include "Types/Events.h"

class TPianoRollEvent : public QEventImpl::EditorUpdateEvent {
public:
    TPianoRollEvent(int type);
    ~TPianoRollEvent();

    enum PianoUpdateType {
        Operate,
        Select,
        ChangeTimeSig,
        ChangeTempo,
    };

    inline int pType() const {
        return pt;
    }

    void dispatch(QObject *current);

protected:
    int pt;
};

#endif // TPIANOROLLEVENT_H
