#include "TNPlayhead.h"

#include <QPen>

TNPlayhead::TNPlayhead(TNotesArea *area, QGraphicsItem *parent) : TNRectObject(area, parent) {
    setPen(Qt::NoPen);
}

TNPlayhead::~TNPlayhead() {
}

double TNPlayhead::left() const {
    return x() + rect().x();
}

double TNPlayhead::right() const {
    QRectF rect = this->rect();
    return x() + rect.x() + rect.width();
}

QPainterPath TNPlayhead::shape() const {
    return QPainterPath();
}

bool TNPlayhead::alwaysShow() const {
    return m_alwaysShow;
}

void TNPlayhead::setAlwaysShow(bool alwaysShow) {
    m_alwaysShow = alwaysShow;
    if (!m_alwaysShow) {
        setVisible(m_playing);
    } else if (!isVisible()) {
        setVisible(true);
    }
}

bool TNPlayhead::playing() const {
    return m_playing;
}

void TNPlayhead::setPlaying(bool playing) {
    m_playing = playing;
    if (!m_alwaysShow) {
        setVisible(m_playing);
    } else if (!isVisible()) {
        setVisible(true);
    }
}
