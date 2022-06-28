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

EditorRequestEvent::EditorRequestEvent(int t)
    : QEvent(static_cast<QEvent::Type>(EditorRequest)), rt(t) {
}

EditorRequestEvent::~EditorRequestEvent() {
}

StdinRequestEvent::StdinRequestEvent(InputType type, InputProcess process)
    : QEvent(static_cast<QEvent::Type>(StdinRequest)), it(type), ip(process) {
}

StdinRequestEvent::~StdinRequestEvent() {
}

SceneActionEvent::SceneActionEvent(int a)
    : QEvent(static_cast<QEvent::Type>(SceneAction)), at(a) {
}

SceneActionEvent::~SceneActionEvent() {
}

SceneStateChangeEvent::SceneStateChangeEvent(int type)
    : QEvent(static_cast<QEvent::Type>(SceneStateChange)), ct(type) {
}

SceneStateChangeEvent::~SceneStateChangeEvent() {
}

SceneStateQueryEvent::SceneStateQueryEvent(int type) : SceneStateChangeEvent(type) {
    t = static_cast<QEvent::Type>(SceneStateQuery);
}

SceneStateQueryEvent::~SceneStateQueryEvent() {
}

InterruptEvent::InterruptEvent() : QEvent(static_cast<QEvent::Type>(Interrupt)) {
}

InterruptEvent::~InterruptEvent() {
}

void QEventImpl::Register() {
    QEvent::registerEventType(MenuUpdateRequest);
    QEvent::registerEventType(ItemGeometryUpdate);
    QEvent::registerEventType(EditorRequest);
    QEvent::registerEventType(StdinRequest);
    QEvent::registerEventType(SceneAction);
    QEvent::registerEventType(SceneStateChange);
    QEvent::registerEventType(SceneStateQuery);
    QEvent::registerEventType(Interrupt);
    QEvent::registerEventType(MainMenuTrigger);
}

MainMenuTriggerEvent::MainMenuTriggerEvent(int id)
    : QEvent(static_cast<QEvent::Type>(MainMenuTrigger)), id(id) {
}

MainMenuTriggerEvent::~MainMenuTriggerEvent() {
}
