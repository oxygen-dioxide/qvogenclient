#ifndef EVENTS_H
#define EVENTS_H

#include <qevent.h>

namespace QEventImpl {
    enum Event {
        MenuUpdateRequest = QEvent::User + 1000,

        ItemGeometryUpdate,

        EditorRequest,

        StdinRequest,

        SceneAction,

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

    // Editor Request
    class EditorRequestEvent : public QEvent {
    public:
        EditorRequestEvent(int type);
        ~EditorRequestEvent();

        enum RequestType {
            PianoRoll,
        };

        inline int rType() const {
            return rt;
        }

    protected:
        int rt;
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

    // Scene Action
    class SceneActionEvent : public QEvent {
    public:
        enum ActionType {
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

        SceneActionEvent(int a);
        ~SceneActionEvent();

        inline int aType() const {
            return at;
        }

    protected:
        int at;
    };

    // Scene State Change
    class SceneStateChangeEvent : public QEvent {
    public:
        SceneStateChangeEvent(int type);
        ~SceneStateChangeEvent();

        enum ChangeType {
            CursorMode,
            SceneRect,
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
