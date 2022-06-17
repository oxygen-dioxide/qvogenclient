#include "TNRectObject.h"
#include "../TNotesArea.h"

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
