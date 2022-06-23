#include "TNRectObject.h"
#include "../TNotesArea.h"

#include <QDebug>
#include <QEvent>

#include "Types/Events.h"

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

QSizeF TNRectObject::size() const {
    return rect().size();
}

void TNRectObject::setSize(const QSizeF &size) {
    setRect(QRectF(QPointF(0, 0), size));
}

void TNRectObject::setSize(double w, double h) {
    setRect(0, 0, w, h);
}

double TNRectObject::width() const {
    return rect().width();
}

double TNRectObject::height() const {
    return rect().height();
}

void TNRectObject::layoutRequestEvent(QEvent *event) {
    Q_UNUSED(event);
}

bool TNRectObject::event(QEvent *event) {
    if (event->type() == (int) QEventImpl::ItemGeometryUpdate) {
        layoutRequestEvent(event);
    }
    return QObject::event(event);
}

QDebug operator<<(QDebug debug, const TNRectObject *obj) {
    debug << static_cast<const QObject *>(obj);
    return debug;
}
