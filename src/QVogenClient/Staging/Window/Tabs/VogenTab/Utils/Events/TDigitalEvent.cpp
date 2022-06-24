#include "TDigitalEvent.h"

TDigitalEvent::TDigitalEvent(int type) : QEventImpl::SceneActionRequestEvent(Digital), dt(type) {
    digit = 0;
}

TDigitalEvent::~TDigitalEvent() {
}
