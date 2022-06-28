#include "TAppendEvent.h"

TAppendEvent::TAppendEvent() : QEventImpl::SceneActionEvent(Append) {
}

TAppendEvent::~TAppendEvent() {
}
