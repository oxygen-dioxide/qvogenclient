#ifndef EVENTS_H
#define EVENTS_H

#include <qevent.h>

namespace QEventImpl {
    enum Event {
        MenuUpdateRequest = QEvent::User + 1000,
    };

    // Menu Update Request
    class MenuUpdateRequestEvent : public QEvent {
        MenuUpdateRequestEvent(int menuIndex);
        ~MenuUpdateRequestEvent();

    public:
        int menuIndex() const;

    protected:
        int m_menuIndex;
    };

    void Register();

}; // namespace QEventImpl

#endif // EVENTS_H
