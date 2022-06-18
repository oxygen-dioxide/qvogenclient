#include "TNotesScroll.h"
#include "TNotesArea.h"

#include <QApplication>
#include <QScrollBar>
#include <QWheelEvent>

TNotesScroll::ControlData::ControlData() {
    moveH = Qt::ShiftModifier;
    zoomH = Qt::ControlModifier | Qt::ShiftModifier;
    zoomV = Qt::ControlModifier;

    selectS = Qt::ControlModifier;
    selectC = Qt::ShiftModifier;

    dragS = Qt::ControlModifier;
    dragM = Qt::ControlModifier | Qt::ShiftModifier;
    dragZ = Qt::ControlModifier | Qt::AltModifier;

    stretchR = Qt::ControlModifier;
    stretchA = Qt::ControlModifier | Qt::ShiftModifier;
}

TNotesScroll::TNotesScroll(QWidget *parent) : CGraphicsView(parent) {
    setRenderHint(QPainter::Antialiasing);
    setAcceptDrops(false);
}

TNotesScroll::~TNotesScroll() {
}

TNotesScroll::ControlData TNotesScroll::controlData() const {
    return m_controlData;
}

void TNotesScroll::setControlData(const ControlData &data) {
    m_controlData = data;
}

TNotesArea *TNotesScroll::area() const {
    return qobject_cast<TNotesArea *>(scene());
}

bool TNotesScroll::event(QEvent *event) {
    //    switch (event->type()) {
    //    case QEvent::FontChange: {
    //        auto area = this->area();
    //        if (area) {
    //            area->setFont(this->font());
    //        }
    //        break;
    //    }
    //    default:
    //        break;
    //    }
    return CGraphicsView::event(event);
}

void TNotesScroll::wheelEvent(QWheelEvent *event) {
    Qt::KeyboardModifiers c = event->modifiers();
    QPoint delta = event->angleDelta();
    bool isTouch = event->source() == Qt::MouseEventSynthesizedBySystem;

    if (c & Qt::AltModifier) {
        return;
    }
    if (c == Qt::NoModifier) {
        if (isTouch) {
            event->setModifiers(Qt::NoModifier);
            return CGraphicsView::wheelEvent(event);
        }
        QApplication::sendEvent(verticalScrollBar(), event);
    } else if (c == m_controlData.moveH) {
        if (isTouch) {
            event->setModifiers(Qt::NoModifier);
            return CGraphicsView::wheelEvent(event);
        }
        event->setModifiers(Qt::AltModifier);
        QApplication::sendEvent(horizontalScrollBar(), event);
    } else if (c == m_controlData.zoomH) {
        if (area()->scrollZoomAllowed()) {
            emitZoomRequest(Qt::Horizontal,
                            qAbs(delta.x()) > qAbs(delta.y()) ? delta.x() : delta.y(),
                            QCursor::pos());
        }
    } else if (c == m_controlData.zoomV) {
        if (area()->scrollZoomAllowed()) {
            emitZoomRequest(Qt::Vertical, qAbs(delta.x()) > qAbs(delta.y()) ? delta.x() : delta.y(),
                            QCursor::pos());
        }
    }
}

void TNotesScroll::scrollContentsBy(int dx, int dy) {
    CGraphicsView::scrollContentsBy(dx, dy);
    emit viewMoved(dx, dy);
}

void TNotesScroll::emitZoomRequest(Qt::Orientation orient, int offset, const QPoint &anchor) {
    emit zoomRequested(orient, offset, anchor);
}
