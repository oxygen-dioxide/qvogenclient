#include "TSSCSceneRectEvent.h"

TSSCSceneRectEvent::TSSCSceneRectEvent()
    : QEventImpl::SceneStateChangeEvent(QEventImpl::SceneStateChangeEvent::SceneRect) {
    sections = oldSections = 0;
}

TSSCSceneRectEvent::~TSSCSceneRectEvent() {
}
