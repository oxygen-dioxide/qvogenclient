#include "TNEdgeSprite.h"

TNEdgeSprite::TNEdgeSprite(TNotesArea *area, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent), m_area(area) {
}

TNEdgeSprite::~TNEdgeSprite() {
}

TNotesArea *TNEdgeSprite::area() const {
    return m_area;
}

QPainterPath TNEdgeSprite::shape() const {
    return QPainterPath();
}
