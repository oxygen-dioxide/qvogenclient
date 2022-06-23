#include "TNRectSelectable.h"

TNRectSelectable::TNRectSelectable(TNotesArea *area, QGraphicsItem *parent)
    : TNRectObject(area, parent) {
    m_selected = false;
    m_enabled = true;

    m_begin = 0;
    m_end = 0;

    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

TNRectSelectable::~TNRectSelectable() {
}

bool TNRectSelectable::isSelected() const {
    return m_selected;
}

void TNRectSelectable::setSelected(bool selected) {
    if (m_selected != selected) {
        m_selected = selected;
        update();
    }
}

bool TNRectSelectable::isEnabled() const {
    return m_enabled;
}

void TNRectSelectable::setEnabled(bool enabled) {
    if (m_enabled != enabled) {
        m_enabled = enabled;
        update();
    }
}

int TNRectSelectable::begin() const {
    return m_begin;
}

void TNRectSelectable::setBegin(int begin) {
    int org = m_begin;
    m_begin = begin;
    if (m_begin != org) {
        emit beginChanged(m_begin, org);
    }
}

int TNRectSelectable::end() const {
    return m_end;
}

void TNRectSelectable::setEnd(int end) {
    int org = m_end;
    m_end = end;
    if (m_end != org) {
        emit beginChanged(m_end, org);
    }
}
