#ifndef TAPPENDEVENT_H
#define TAPPENDEVENT_H

#include "Types/Events.h"

#include "../TWrappedData.h"

class TAppendEvent : public QEventImpl::SceneActionEvent {
public:
    TAppendEvent();
    ~TAppendEvent();

    QList<TWProject::Utterance> utterances;
};

#endif // TAPPENDEVENT_H
