#include "TSSQCursorModeEvent.h"

TSSQCursorModeEvent::TSSQCursorModeEvent()
    : QEventImpl::SceneStateQueryEvent(QEventImpl::SceneStateChangeEvent::CursorMode) {
}

TSSQCursorModeEvent::~TSSQCursorModeEvent() {
}
