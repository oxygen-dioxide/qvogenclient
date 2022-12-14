#include "TNScreenCtl.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Events.h"

TNScreenCtl::TNScreenCtl(TNotesArea *parent) : TNController(parent) {
}

TNScreenCtl::~TNScreenCtl() {
}

void TNScreenCtl::install() {
    a->installEventFilter(this);

    m_screen = new TNRectScreen(a);
    a->addItem(m_screen);
    m_screen->setZValue(TNotesArea::Lines);
}

void TNScreenCtl::updateScreen() {
    QRectF rect = a->view()->viewportRect();
    m_screen->setPos(rect.topLeft());
    m_screen->setRect(QRectF(QPointF(0, 0), rect.size()));
    m_screen->update();
}

bool TNScreenCtl::eventFilter(QObject *obj, QEvent *event) {
    if (obj == a) {
        switch (event->type()) {
        case QEvent::GraphicsSceneMove:
        case QEvent::GraphicsSceneResize:
            updateScreen();
            break;
        case QEventImpl::SceneStateChange: {
            auto e = static_cast<QEventImpl::SceneStateChangeEvent *>(event);
            // switch type 2
            switch (e->cType()) {
            case QEventImpl::SceneStateChangeEvent::SceneRect:
                updateScreen();
                break;
            default:
                break;
            }
            // end switch type 2

            break;
        }
        default:
            break;
        }
    }
    return TNController::eventFilter(obj, event);
}
