#include "TDigitalEvent.h"

TDigitalEvent::TDigitalEvent(int type) : QEventImpl::SceneActionEvent(Digital), dt(type) {
    digit = 0;
}

TDigitalEvent::~TDigitalEvent() {
}
