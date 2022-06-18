#include "TNPriorCtl.h"
#include "../TNotesArea.h"

#include "Types/Events.h"

TNPriorCtl::TNPriorCtl(TNotesArea *parent) : TNController(parent) {
}

TNPriorCtl::~TNPriorCtl() {
}

void TNPriorCtl::install() {
    m_itemUnderMouse = nullptr;

    a->installEventFilter(this);
}

QGraphicsItem *TNPriorCtl::itemUnderMouse() const {
    return m_itemUnderMouse;
}

bool TNPriorCtl::eventFilter(QObject *obj, QEvent *event) {
    if (obj == a) {
        switch (event->type()) {
        case QEvent::GraphicsSceneMousePress: {
            auto e = static_cast<QGraphicsSceneMouseEvent *>(event);
            m_itemUnderMouse = a->itemAt(e->scenePos(), QTransform());
            break;
        }
        default:
            break;
        }
    }
    return TNController::eventFilter(obj, event);
}
