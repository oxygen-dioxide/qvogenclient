#include "TNPriorCtl.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "CMenu.h"
#include "Types/Events.h"

#include <QApplication>
#include <QDebug>

TNPriorCtl::TNPriorCtl(TNotesArea *parent) : TNController(parent) {
}

TNPriorCtl::~TNPriorCtl() {
}

void TNPriorCtl::install() {
    m_itemUnderMouse = nullptr;
    m_downButton = Qt::NoButton;

    a->installEventFilter(this);
}

QGraphicsItem *TNPriorCtl::itemUnderMouse() const {
    return m_itemUnderMouse;
}

void TNPriorCtl::openContextMenu() {
    CMenu menu(a->view()->window());

    QAction newGroupAction(tr("Move to New Utterance"), &menu);
    QAction dissolveGroupAction(tr("Dissolve from Utterance"), &menu);
    QAction cutAction(tr("Cut"), &menu);
    QAction copyAction(tr("Copy"), &menu);
    QAction pasteAction(tr("Paste"), &menu);
    QAction removeAction(tr("Remove"), &menu);
    QAction selectAllAction(tr("Select All"), &menu);
    QAction deselectAction(tr("Deselect"), &menu);

    menu.addAction(&newGroupAction);
    menu.addAction(&dissolveGroupAction);
    menu.addSeparator();
    menu.addAction(&cutAction);
    menu.addAction(&copyAction);
    menu.addAction(&pasteAction);
    menu.addAction(&removeAction);
    menu.addSeparator();
    menu.addAction(&selectAllAction);
    menu.addAction(&deselectAction);

    if (!a->hasSelection()) {
        newGroupAction.setDisabled(true);
        dissolveGroupAction.setDisabled(true);
        cutAction.setDisabled(true);
        copyAction.setDisabled(true);
        removeAction.setDisabled(true);
        deselectAction.setDisabled(true);
    }

    if (a->currentGroupId() == 1) {
        dissolveGroupAction.setDisabled(true);
    }

    auto action = menu.exec(QCursor::pos());

    if (action == &newGroupAction) {
        QEventImpl::SceneActionEvent e(QEventImpl::SceneActionEvent::Group);
        qApp->sendEvent(a, &e);
    } else if (action == &dissolveGroupAction) {
        QEventImpl::SceneActionEvent e(QEventImpl::SceneActionEvent::Ungroup);
        qApp->sendEvent(a, &e);
    } else if (action == &cutAction) {
        QEventImpl::SceneActionEvent e(QEventImpl::SceneActionEvent::Cut);
        qApp->sendEvent(a, &e);
    } else if (action == &copyAction) {
        QEventImpl::SceneActionEvent e(QEventImpl::SceneActionEvent::Copy);
        qApp->sendEvent(a, &e);
    } else if (action == &pasteAction) {
        QEventImpl::SceneActionEvent e(QEventImpl::SceneActionEvent::Paste);
        qApp->sendEvent(a, &e);
    } else if (action == &removeAction) {
        QEventImpl::SceneActionEvent e(QEventImpl::SceneActionEvent::Remove);
        qApp->sendEvent(a, &e);
    } else if (action == &selectAllAction) {
        QEventImpl::SceneActionEvent e(QEventImpl::SceneActionEvent::SelectAll);
        qApp->sendEvent(a, &e);
    } else if (action == &deselectAction) {
        QEventImpl::SceneActionEvent e(QEventImpl::SceneActionEvent::Deselect);
        qApp->sendEvent(a, &e);
    }
}

bool TNPriorCtl::eventFilter(QObject *obj, QEvent *event) {
    if (obj == a) {
        switch (event->type()) {
        case QEvent::GraphicsSceneMouseDoubleClick:
        case QEvent::GraphicsSceneMousePress: {
            auto e = static_cast<QGraphicsSceneMouseEvent *>(event);
            if (m_downButton) {
                // Stop spreading event when there's already a button down
                return true;
            } else {
                m_downButton = e->button();
                m_itemUnderMouse = a->itemAt(e->scenePos(), QTransform());
            }
            break;
        }
        case QEvent::GraphicsSceneMouseMove: {
            break;
        }
        case QEvent::GraphicsSceneMouseRelease: {
            auto e = static_cast<QGraphicsSceneMouseEvent *>(event);
            auto btn = e->button();
            if (btn != m_downButton) {
                // Stop spreading event when it's not current button
                return true;
            }
            if (btn == Qt::RightButton) {
                if (!a->isSelecting()) {
                    openContextMenu();
                }
            }
            m_downButton = Qt::NoButton;
            break;
        }

        case QEventImpl::StdinRequest: {
            auto e = static_cast<QEventImpl::StdinRequestEvent *>(event);
            switch (e->iType()) {
            // switch type 2
            case QEventImpl::StdinRequestEvent::Lyrics: {
                // Handle Lyrics Input
                switch (e->iProcess()) {
                case QEventImpl::StdinRequestEvent::InputStart: {
                    // Interrupt
                    sendInterrupt();
                    break;
                }
                default:
                    break;
                }
                break;
            }
            default:
                break;
            }
            // end switch type 2

            break;
        }

        case QEventImpl::SceneAction: {
            auto e = static_cast<QEventImpl::SceneActionEvent *>(event);
            auto act = e->aType();
            switch (act) {
            case QEventImpl::SceneActionEvent::Paste:
            case QEventImpl::SceneActionEvent::Cut:
            case QEventImpl::SceneActionEvent::Remove:
            case QEventImpl::SceneActionEvent::Append:
            case QEventImpl::SceneActionEvent::Digital:
            case QEventImpl::SceneActionEvent::Group:
            case QEventImpl::SceneActionEvent::Ungroup: {
                // Interrupt
                sendInterrupt();
                break;
            }
            case QEventImpl::SceneActionEvent::Copy:
            case QEventImpl::SceneActionEvent::SelectAll: {
            case QEventImpl::SceneActionEvent::Deselect: {
                break;
            }
            default:
                break;
            }
            }
            break;
        }
        default:
            break;
        }
    }
    return TNController::eventFilter(obj, event);
}
