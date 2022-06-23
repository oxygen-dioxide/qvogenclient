#include "TNSelectCtl.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Events.h"

#include <QApplication>

TNSelectCtl::TNSelectCtl(TNotesArea *parent) : TNController(parent) {
}

TNSelectCtl::~TNSelectCtl() {
}

void TNSelectCtl::install() {
    a->installEventFilter(this);

    m_rubber = new TNRubber(a);
    a->addItem(m_rubber);
    m_rubber->setZValue(TNotesArea::RubberBand);
}

bool TNSelectCtl::isSelecting() const {
    return m_rubber->active();
}

void TNSelectCtl::stopSelecting() {
    QRectF res;
    m_rubber->stop(&res);

    QEventImpl::SceneRubberSelectEvent e(res);
    QApplication::sendEvent(a, &e);
}

bool TNSelectCtl::eventFilter(QObject *obj, QEvent *event) {
    if (obj == a) {
        switch (event->type()) {

            // Mouse Press Event
            case QEvent::GraphicsSceneMousePress: {
                break;
            }

                // Mouse Move Event
            case QEvent::GraphicsSceneMouseMove: {
                auto item = a->itemUnderMouse();
                // No Item Under Mouse
                if (!item) {
                    auto e = static_cast<QGraphicsSceneMouseEvent *>(event);
                    auto buttons = e->buttons();
                    if (!a->mouseMoving()) {
                        // Mouse About To Move
                        if (!a->visionMoving()) {
                            bool selecting = false;
                            const auto &data = a->view()->controlData();
                            const auto &modifiers = qApp->keyboardModifiers();
                            if (modifiers == data.dragS) {
                                selecting = true;
                            } else if (modifiers == Qt::NoModifier) {
                                switch (a->drawMode()) {
                                    case TNotesArea::PlainSelect: {
                                        selecting = true;
                                        break;
                                    }
                                    default:
                                        break;
                                }
                            }
                            if (selecting) {
                                // Decision: Select
                                if (!m_rubber->active()) {
                                    if (buttons == Qt::LeftButton) {
                                        m_rubber->start();
                                    } else if (buttons == Qt::RightButton) {
                                        m_rubber->start(true);
                                    }
                                }
                            }
                        }
                    } else {
                        // Mouse Moving
                    }
                }
                break;
            }

            // Mouse Release Event
            case QEvent::GraphicsSceneMouseRelease: {
                if (m_rubber->active()) {
                    stopSelecting();
                }
                break;
            }

            default:
                break;
        }
    }
    return TNController::eventFilter(obj, event);
}
