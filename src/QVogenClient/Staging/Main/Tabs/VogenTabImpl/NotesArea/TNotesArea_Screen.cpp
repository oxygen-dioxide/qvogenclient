#include "TNotesArea.h"
#include "TNotesScroll.h"

void TNotesArea::updateScreen() {
    QRectF rect = m_view->viewportRect();
    m_screen->setPos(rect.topLeft());
    m_screen->setRect(QRectF(QPointF(0, 0), rect.size()));
    m_screen->update();
}

void TNotesArea::initScreenElements() {
    m_screen = new TNRectScreen(this);
    addItem(m_screen);
    m_screen->setZValue(linesZIndex);
}
