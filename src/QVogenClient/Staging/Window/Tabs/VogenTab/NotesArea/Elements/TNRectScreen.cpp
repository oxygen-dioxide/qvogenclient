#include "TNRectScreen.h"

#include <QPainter>

TNRectScreen::TNRectScreen(TNotesArea *area, QGraphicsItem *parent) : TNRectObject(area, parent) {
}

TNRectScreen::~TNRectScreen() {
}

QPainterPath TNRectScreen::shape() const {
    return QPainterPath();
}

void TNRectScreen::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                         QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!m_pixmap.isNull()) {
        painter->drawPixmap(this->rect().toRect(), m_pixmap);
    }
}

QPixmap TNRectScreen::pixmap() const {
    return m_pixmap;
}

void TNRectScreen::setPixmap(const QPixmap &pixmap) {
    m_pixmap = pixmap;
    update();
}
