#ifndef TSSCSCENERECTEVENT_H
#define TSSCSCENERECTEVENT_H

#include "Types/Events.h"

class TSSCSceneRectEvent : public QEventImpl::SceneStateChangeEvent {
public:
    TSSCSceneRectEvent();
    ~TSSCSceneRectEvent();

    QSize size, oldSize;
    int sections, oldSections;
};

#endif // TSSCSCENERECTEVENT_H
