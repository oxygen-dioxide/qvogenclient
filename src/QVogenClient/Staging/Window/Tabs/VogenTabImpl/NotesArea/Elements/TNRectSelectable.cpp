#include "TNRectSelectable.h"

TNRectSelectable::TNRectSelectable(TNotesArea *area, QGraphicsItem *parent)
    : TNRectObject(area, parent) {
    m_selected = false;
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

TNRectSelectable::~TNRectSelectable() {
}

bool TNRectSelectable::isSelected() const {
    return m_selected;
}

void TNRectSelectable::setSelected(bool selected) {
    m_selected = selected;
    update();
}
