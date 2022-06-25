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
        QEventImpl::SceneActionRequestEvent e(QEventImpl::SceneActionRequestEvent::Group);
        qApp->sendEvent(a, &e);
    } else if (action == &dissolveGroupAction) {
        QEventImpl::SceneActionRequestEvent e(QEventImpl::SceneActionRequestEvent::Ungroup);
        qApp->sendEvent(a, &e);
    } else if (action == &cutAction) {
        QEventImpl::SceneActionRequestEvent e(QEventImpl::SceneActionRequestEvent::Cut);
        qApp->sendEvent(a, &e);
    } else if (action == &copyAction) {
        QEventImpl::SceneActionRequestEvent e(QEventImpl::SceneActionRequestEvent::Copy);
        qApp->sendEvent(a, &e);
    } else if (action == &pasteAction) {
        QEventImpl::SceneActionRequestEvent e(QEventImpl::SceneActionRequestEvent::Paste);
        qApp->sendEvent(a, &e);
    } else if (action == &removeAction) {
        QEventImpl::SceneActionRequestEvent e(QEventImpl::SceneActionRequestEvent::Remove);
        qApp->sendEvent(a, &e);
    } else if (action == &selectAllAction) {
        QEventImpl::SceneActionRequestEvent e(QEventImpl::SceneActionRequestEvent::SelectAll);
        qApp->sendEvent(a, &e);
    } else if (action == &deselectAction) {
        QEventImpl::SceneActionRequestEvent e(QEventImpl::SceneActionRequestEvent::Deselect);
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
        default:
            break;
        }
    }
    return TNController::eventFilter(obj, event);
}
