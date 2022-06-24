#include "Events.h"

QEventImpl::MenuUpdateRequestEvent::MenuUpdateRequestEvent(int menuIndex)
    : QEvent(static_cast<QEvent::Type>(MenuUpdateRequest)), m_menuIndex(menuIndex) {
}

QEventImpl::MenuUpdateRequestEvent::~MenuUpdateRequestEvent() {
}

int QEventImpl::MenuUpdateRequestEvent::menuIndex() const {
    return m_menuIndex;
}

QEventImpl::SceneRectChangeEvent::SceneRectChangeEvent(const QPair<QSize, int> &cur,
                                                       const QPair<QSize, int> &org)
    : QResizeEvent(cur.first, org.first), sec(cur.second), oldSec(org.second) {
    this->t = static_cast<QEvent::Type>(SceneRectChange);
}

QEventImpl::SceneRectChangeEvent::~SceneRectChangeEvent() {
}

bool QEventImpl::SceneRectChangeEvent::sizeChanged() const {
    return s != olds;
}

QEventImpl::SceneRubberSelectEvent::SceneRubberSelectEvent(const QRectF &rect)
    : QEvent(static_cast<QEvent::Type>(SceneRubberSelect)), r(rect) {
}

QEventImpl::SceneRubberSelectEvent::~SceneRubberSelectEvent() {
}

QEventImpl::PianoRollChangeEvent::PianoRollChangeEvent(int t)
    : QEvent(static_cast<QEvent::Type>(PianoRollChange)), t(t) {
}

QEventImpl::PianoRollChangeEvent::~PianoRollChangeEvent() {
}

QEventImpl::StdinRequestEvent::StdinRequestEvent(InputType type, InputProcess process)
    : QEvent(static_cast<QEvent::Type>(StdinRequest)), it(type), ip(process) {
}

QEventImpl::StdinRequestEvent::~StdinRequestEvent() {
}

void QEventImpl::Register() {
    QEvent::registerEventType(MenuUpdateRequest);
    QEvent::registerEventType(SceneRectChange);
    QEvent::registerEventType(SceneRubberSelect);
    QEvent::registerEventType(ItemGeometryUpdate);
    QEvent::registerEventType(PianoRollChange);
    QEvent::registerEventType(StdinRequest);
}
