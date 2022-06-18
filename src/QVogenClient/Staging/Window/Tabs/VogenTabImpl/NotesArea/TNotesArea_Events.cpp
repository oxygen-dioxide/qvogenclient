#include "Elements/TNRubber.h"
#include "TNotesArea.h"
#include "TNotesScroll.h"

#include <QApplication>
#include <QDebug>
#include <QFocusEvent>
#include <QGraphicsSceneMouseEvent>

#include "Types/Events.h"

void TNotesArea::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    CGraphicsScene::mousePressEvent(event);
}

void TNotesArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    // Ensure event triggered by real mouse moving
    if (event->buttons()) {
        // First movement after move
        if (!m_moving) {
            CGraphicsScene::mouseMoveEvent(event);
            m_moving = true;

            QApplication::sendEvent(this, event);
            return;
        }
    }
    CGraphicsScene::mouseMoveEvent(event);
}

void TNotesArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    m_moving = false;
    CGraphicsScene::mouseReleaseEvent(event);
}

void TNotesArea::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    CGraphicsScene::mouseDoubleClickEvent(event);
}

void TNotesArea::focusOutEvent(QFocusEvent *event) {
    m_moving = false;
    CGraphicsScene::focusOutEvent(event);
}

void TNotesArea::_q_sceneRectChanged(const QRectF &rect) {
    Q_UNUSED(rect)

    auto curSizes = qMakePair(QSize(currentWidth(), currentHeight()), sectionCount());
    auto oldSizes = this->m_oldSizes;

    m_oldSizes = curSizes;

    QEventImpl::SceneRectChangeEvent e(curSizes, oldSizes);
    QApplication::sendEvent(this, &e);

    adjustBackground();
}
