#include "Events.h"

using namespace QEventImpl;

MenuUpdateRequestEvent::MenuUpdateRequestEvent(int menuIndex)
    : QEvent(static_cast<QEvent::Type>(MenuUpdateRequest)), m_menuIndex(menuIndex) {
}

MenuUpdateRequestEvent::~MenuUpdateRequestEvent() {
}

int MenuUpdateRequestEvent::menuIndex() const {
    return m_menuIndex;
}

SceneRectChangeEvent::SceneRectChangeEvent(const QPair<QSize, int> &cur,
                                           const QPair<QSize, int> &org)
    : QResizeEvent(cur.first, org.first), sec(cur.second), oldSec(org.second) {
    this->t = static_cast<QEvent::Type>(SceneRectChange);
}

SceneRectChangeEvent::~SceneRectChangeEvent() {
}

bool SceneRectChangeEvent::sizeChanged() const {
    return s != olds;
}

SceneRubberSelectEvent::SceneRubberSelectEvent(const QRectF &rect)
    : QEvent(static_cast<QEvent::Type>(SceneRubberSelect)), r(rect) {
}

SceneRubberSelectEvent::~SceneRubberSelectEvent() {
}

PianoRollChangeEvent::PianoRollChangeEvent(int t)
    : QEvent(static_cast<QEvent::Type>(PianoRollChange)), ct(t) {
}

PianoRollChangeEvent::~PianoRollChangeEvent() {
}

StdinRequestEvent::StdinRequestEvent(InputType type, InputProcess process)
    : QEvent(static_cast<QEvent::Type>(StdinRequest)), it(type), ip(process) {
}

StdinRequestEvent::~StdinRequestEvent() {
}

SceneActionRequestEvent::SceneActionRequestEvent(int a)
    : QEvent(static_cast<QEvent::Type>(SceneActionRequest)), a(a) {
}

SceneActionRequestEvent::~SceneActionRequestEvent() {
}

SceneStateChangeEvent::SceneStateChangeEvent(int type)
    : QEvent(static_cast<QEvent::Type>(SceneStateChange)), ct(type) {
}

SceneStateChangeEvent::~SceneStateChangeEvent() {
}

void QEventImpl::Register() {
    QEvent::registerEventType(MenuUpdateRequest);
    QEvent::registerEventType(SceneRectChange);
    QEvent::registerEventType(SceneRubberSelect);
    QEvent::registerEventType(ItemGeometryUpdate);
    QEvent::registerEventType(PianoRollChange);
    QEvent::registerEventType(StdinRequest);
    QEvent::registerEventType(SceneActionRequest);
    QEvent::registerEventType(SceneStateChange);
}
