#include "TNRectObject.h"
#include "../TNotesArea.h"

#include <QEvent>

TNRectObject::TNRectObject(TNotesArea *area, QGraphicsItem *parent)
    : QObject(area), QGraphicsRectItem(parent), m_area(area) {
    init();
}

TNRectObject::~TNRectObject() {
}

void TNRectObject::init() {
}

TNotesArea *TNRectObject::area() const {
    return m_area;
}

void TNRectObject::layoutRequestEvent(QEvent *event) {
    Q_UNUSED(event);
}

bool TNRectObject::event(QEvent *event) {
    if (event->type() == QEvent::LayoutRequest) {
        layoutRequestEvent(event);
    }
    return QObject::event(event);
}
