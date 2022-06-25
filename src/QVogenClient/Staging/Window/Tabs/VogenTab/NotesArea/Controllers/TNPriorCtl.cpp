#include "TNPriorCtl.h"
#include "../TNotesArea.h"

#include "Types/Events.h"

#include <QApplication>
#include <QDebug>

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
            if (QApplication::mouseButtons() & ~e->button()) {
                // Stop spreading event when virtual pressing state is set
                qDebug() << "Press filtered";
            } else {
                auto e = static_cast<QGraphicsSceneMouseEvent *>(event);
                m_itemUnderMouse = a->itemAt(e->scenePos(), QTransform());
            }
            break;
        }
        case QEvent::GraphicsSceneMouseMove: {
            break;
        }
        case QEvent::GraphicsSceneMouseRelease: {
            break;
        }
        default:
            break;
        }
    }
    return TNController::eventFilter(obj, event);
}
