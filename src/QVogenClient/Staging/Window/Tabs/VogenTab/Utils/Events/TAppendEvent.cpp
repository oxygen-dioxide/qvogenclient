#include "TAppendEvent.h"

TAppendEvent::TAppendEvent() : QEventImpl::SceneActionRequestEvent(Append) {
}

TAppendEvent::~TAppendEvent() {
}
