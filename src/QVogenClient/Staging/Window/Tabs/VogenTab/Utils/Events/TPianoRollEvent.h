#ifndef TPIANOROLLEVENT_H
#define TPIANOROLLEVENT_H

#include "Types/Events.h"

class TPianoRollEvent : public QEventImpl::EditorRequestEvent {
public:
    TPianoRollEvent(int type);
    ~TPianoRollEvent();

    enum PianoUpdateType {
        RubberBand,
        Select,
        ChangeTimeSig,
        ChangeTempo,
        Operate,
        ChangeVoice,
        PlayState,
        SetPlayhead,
    };

    inline int pType() const {
        return pt;
    }

    void dispatch(QObject *current);

protected:
    int pt;
};

#endif // TPIANOROLLEVENT_H
