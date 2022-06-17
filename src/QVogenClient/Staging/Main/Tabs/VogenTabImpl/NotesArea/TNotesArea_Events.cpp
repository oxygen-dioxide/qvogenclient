#include "Elements/TNRubber.h"
#include "TNotesArea.h"
#include "TNotesScroll.h"

#include <QApplication>
#include <QDebug>
#include <QFocusEvent>
#include <QGraphicsSceneMouseEvent>

void TNotesArea::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    auto button = event->button();
    const auto &data = m_view->controlData();
    const auto &modifiers = qApp->keyboardModifiers();

    if (button == Qt::LeftButton) {
        if (modifiers == data.dragM) {
            // Decision: Move
            setScrollDrag(true);
        } else if (modifiers == data.dragZ) {
            // Decision: Zoom
            setZoomDrag(true);
        }
    }
    // Other
    CGraphicsScene::mousePressEvent(event);
}

void TNotesArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    auto buttons = event->buttons();
    const auto &data = m_view->controlData();
    const auto &modifiers = qApp->keyboardModifiers();

    // First movement after move
    if (!m_moving) {
        if (!m_scrollDrag && !m_zoomDrag) {
            bool selecting = false;
            if (modifiers == data.dragS) {
                selecting = true;
            } else if (modifiers == Qt::NoModifier) {
                switch (m_drawMode) {
                case PlainSelect: {
                    selecting = true;
                    break;
                }
                case DrawNote: {
                    break;
                }
                case DrawCurves: {
                    break;
                }
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
        m_moving = true;
    }
    if (m_moving) {
        if (m_scrollDrag) {
            // Give control to QT
        } else if (m_zoomDrag) {
            auto sub = event->screenPos() - event->lastScreenPos();
            sub /= 2;
            if (!sub.isNull()) {
                if (qAbs(sub.x()) > qAbs(sub.y())) {
                    m_view->emitZoomRequest(Qt::Horizontal, sub.x(), m_zoomAnchor);
                } else {
                    m_view->emitZoomRequest(Qt::Vertical, sub.y(), m_zoomAnchor);
                }
            }
            return;
        }
    }
    // Other
    CGraphicsScene::mouseMoveEvent(event);
}

void TNotesArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    settleMouseUp();
    CGraphicsScene::mouseReleaseEvent(event);
}

void TNotesArea::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    CGraphicsScene::mouseDoubleClickEvent(event);
}

void TNotesArea::focusInEvent(QFocusEvent *event) {
    CGraphicsScene::focusInEvent(event);
}

void TNotesArea::focusOutEvent(QFocusEvent *event) {
    settleMouseUp();
    CGraphicsScene::focusOutEvent(event);
}

void TNotesArea::viewMoveEvent(QGraphicsSceneMoveEvent *event) {
    Q_UNUSED(event)
    rearrangeElements();
}

void TNotesArea::viewResizeEvent(QGraphicsSceneResizeEvent *event) {
    Q_UNUSED(event)
    rearrangeElements();
}

void TNotesArea::handleSceneRectChanged(const QRectF &rect) {
    Q_UNUSED(rect)
    adjustBackground();
    rearrangeElements();
}

void TNotesArea::rearrangeElements() {
    updatePlayhead();
    updateSprite();
    updateScreen();
}

void TNotesArea::settleMouseUp() {
    if (m_rubber->active()) {
        stopSelecting();
    }
    if (m_scrollDrag) {
        setScrollDrag(false);
    }
    if (m_zoomDrag) {
        setZoomDrag(false);
    }
    m_moving = false;
}
