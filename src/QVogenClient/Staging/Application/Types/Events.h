#ifndef EVENTS_H
#define EVENTS_H

#include <qevent.h>

namespace QEventImpl {
    enum Event {
        MenuUpdateRequest = QEvent::User + 1000,

        SceneRectChange,

        SceneRubberSelect,

        ItemGeometryUpdate,

        EditorUpdate,

        StdinRequest,

        SceneActionRequest,

        SceneStateChange,

        SceneStateQuery,

        Interrupt,

        MainMenuTrigger,

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

    // Scene Rubber Band Select
    class SceneRubberSelectEvent : public QEvent {
    public:
        SceneRubberSelectEvent(const QRectF &rect);
        ~SceneRubberSelectEvent();

        inline const QRectF &rect() const {
            return r;
        }

    protected:
        QRectF r;
    };

    // Piano Roll Change
    class EditorUpdateEvent : public QEvent {
    public:
        EditorUpdateEvent(int type);
        ~EditorUpdateEvent();

        enum UpdateType {
            PianoRoll,
        };

        inline int uType() const {
            return ut;
        }

    protected:
        int ut;
    };

    // Stdin Request
    class StdinRequestEvent : public QEvent {
    public:
        enum InputType {
            Lyrics,
        };

        enum InputProcess {
            InputStart,
            InputUpdate,
            InputCommit,
            InputAbort,
        };

        StdinRequestEvent(InputType type, InputProcess process = InputUpdate);
        ~StdinRequestEvent();

        inline int iType() const {
            return it;
        }
        inline int iProcess() const {
            return ip;
        }

        QString text;

    protected:
        int it;
        int ip;
    };

    // Scene Action Request
    class SceneActionRequestEvent : public QEvent {
    public:
        enum Action {
            Cut,
            Copy,
            Paste,
            Remove,
            SelectAll,
            Deselect,
            Group,
            Ungroup,
            Digital,
            Append,
        };

        SceneActionRequestEvent(int a);
        ~SceneActionRequestEvent();

        inline int action() const {
            return a;
        }

    protected:
        int a;
    };

    // Scene State Change
    class SceneStateChangeEvent : public QEvent {
    public:
        SceneStateChangeEvent(int type);
        ~SceneStateChangeEvent();

        enum ChangeType {
            CursorMode,
            TimeSig,
            Tempo,
        };

        inline int cType() const {
            return ct;
        }

    protected:
        int ct;
    };

    // Scene State Request
    class SceneStateQueryEvent : public SceneStateChangeEvent {
    public:
        SceneStateQueryEvent(int type);
        ~SceneStateQueryEvent();
    };

    // Interrupt
    class InterruptEvent : public QEvent {
    public:
        InterruptEvent();
        ~InterruptEvent();
    };

    // Interrupt
    class MainMenuTriggerEvent : public QEvent {
    public:
        MainMenuTriggerEvent(int id);
        ~MainMenuTriggerEvent();

        inline int actionId() {
            return id;
        }

    protected:
        int id;
    };

    void Register();

}; // namespace QEventImpl

#endif // EVENTS_H
