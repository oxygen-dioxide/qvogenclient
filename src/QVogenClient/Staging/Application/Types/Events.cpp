#include "Events.h"

QEventImpl::MenuUpdateRequestEvent::MenuUpdateRequestEvent(int menuIndex)
    : QEvent(static_cast<QEvent::Type>(MenuUpdateRequest)), m_menuIndex(menuIndex) {
}

QEventImpl::MenuUpdateRequestEvent::~MenuUpdateRequestEvent() {
}

int QEventImpl::MenuUpdateRequestEvent::menuIndex() const {
    return m_menuIndex;
}

void QEventImpl::Register() {
    QEvent::registerEventType(MenuUpdateRequest);
    QEvent::registerEventType(SceneRectChange);
}
