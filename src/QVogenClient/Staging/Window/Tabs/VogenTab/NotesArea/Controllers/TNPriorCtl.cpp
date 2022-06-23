#include "TNPriorCtl.h"
#include "../TNotesArea.h"

#include "Types/Events.h"

#include <QDebug>

TNPriorCtl::TNPriorCtl(TNotesArea *parent) : TNController(parent) {
}

TNPriorCtl::~TNPriorCtl() {
}

void TNPriorCtl::install() {
    m_pressed = false;
    m_itemUnderMouse = nullptr;

    a->installEventFilter(this);
}

bool TNPriorCtl::isMousePressed() const {
    return m_pressed;
}

QGraphicsItem *TNPriorCtl::itemUnderMouse() const {
    return m_itemUnderMouse;
}

bool TNPriorCtl::eventFilter(QObject *obj, QEvent *event) {
    if (obj == a) {
        switch (event->type()) {
            case QEvent::GraphicsSceneMousePress: {
                if (m_pressed) {
                    // Stop spreading event when virtual pressing state is set
                    qDebug() << "Press filtered";
                    return true;
                }
                m_pressed = true;

                auto e = static_cast<QGraphicsSceneMouseEvent *>(event);
                m_itemUnderMouse = a->itemAt(e->scenePos(), QTransform());
                break;
            }
            case QEvent::GraphicsSceneMouseMove: {
            }
            case QEvent::GraphicsSceneMouseRelease: {
                m_pressed = false;
            }
            default:
                break;
        }
    }
    return TNController::eventFilter(obj, event);
}
