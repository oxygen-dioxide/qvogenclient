#ifndef TCHANGEVOICEEVENT_H
#define TCHANGEVOICEEVENT_H

#include "../TPianoRollEvent.h"

class TChangeVoiceEvent : public TPianoRollEvent {
public:
    TChangeVoiceEvent(int type);
    ~TChangeVoiceEvent();

    enum VoiceType {
        Singer,
        Rom,
    };

    inline int vType() {
        return vt;
    }

public:
    QString singerId;
    QString rom;
    int groupId;

protected:
    int vt;
};

#endif // TCHANGEVOICEEVENT_H
