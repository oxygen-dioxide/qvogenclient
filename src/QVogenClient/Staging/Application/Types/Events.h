#ifndef EVENTS_H
#define EVENTS_H

#include <qevent.h>

namespace QEventImpl {
    enum Event {
        MenuUpdateRequest = QEvent::User + 1000,

        SceneRectChange,

        // LayoutRequest
    };

    // Menu Update Request
    class MenuUpdateRequestEvent : public QEvent {
    public:
        MenuUpdateRequestEvent(int menuIndex);
        ~MenuUpdateRequestEvent();

        int menuIndex() const;

    protected:
        int m_menuIndex;
    };

    // Scene Rect Change
    class SceneRectChangeEvent : public QResizeEvent {
    public:
        SceneRectChangeEvent(const QPair<QSize, int> &cur, const QPair<QSize, int> &org);
        ~SceneRectChangeEvent();

        bool sizeChanged() const;

        inline const int &sections() const {
            return sec;
        }
        inline const int &oldSections() const {
            return oldSec;
        }

    protected:
        int sec, oldSec;
    };

    void Register();

}; // namespace QEventImpl

#endif // EVENTS_H
