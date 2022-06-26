#ifndef TAPPENDEVENT_H
#define TAPPENDEVENT_H

#include "Types/Events.h"

#include "../TWrappedData.h"

class TAppendEvent : public QEventImpl::SceneActionRequestEvent {
public:
    TAppendEvent();
    ~TAppendEvent();

    QList<TWProject::Utterance> utterances;
};

#endif // TAPPENDEVENT_H
