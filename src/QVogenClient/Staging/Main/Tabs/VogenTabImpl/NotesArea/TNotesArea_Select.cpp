#include "TNotesArea.h"
#include "TNotesScroll.h"

void TNotesArea::stopSelecting() {
    QRectF res;
    m_rubber->stop(&res);
}

void TNotesArea::setScrollDrag(bool scrollDrag) {
    m_scrollDrag = scrollDrag;
    m_view->setDragMode(m_scrollDrag ? QGraphicsView::ScrollHandDrag : QGraphicsView::NoDrag);
}

void TNotesArea::setZoomDrag(bool zoomDrag) {
    if (zoomDrag) {
        m_zoomAnchor = QCursor::pos();
    } else {
        m_zoomAnchor = QPoint(0, 0);
    }
    m_zoomDrag = zoomDrag;
}

void TNotesArea::initSelectElements() {
    m_moving = false;
    m_scrollDrag = false;
    m_zoomDrag = false;

    m_rubber = new TNRubber(this);
    addItem(m_rubber);
    m_rubber->setZValue(rubberBandZIndex);
}

bool TNotesArea::isSelecting() const {
    return m_rubber->active();
}

bool TNotesArea::isScrollDragging() const {
    return m_scrollDrag;
}

bool TNotesArea::isZoomDragging() const {
    return m_zoomDrag;
}

bool TNotesArea::scrollZoomAllowed() const {
    return !isSelecting() && !isScrollDragging() && !isZoomDragging();
}
