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
}
