#include "TNotesArea.h"
#include "TNotesScroll.h"

void TNotesArea::updatePlayhead() {
    if (m_playhead->rect().height() != sceneRect().height()) {
        m_playhead->setRect(-1, 0, 2, sceneRect().height());
    }
    m_playhead->setPos(m_playToTick / 480 * m_currentWidth + zeroLine(), 0);
}

void TNotesArea::initPlayElements() {
    m_playToNote = 0;
    m_playToPosition = 0;
    m_playToTick = 0;

    m_playhead = new TNPlayhead(this);
    addItem(m_playhead);
    m_playhead->setZValue(playHeadZIndex);

    m_playhead->setPos(0, 0);
    m_playhead->setAlwaysShow(false);
    m_playhead->setPlaying(false);

    updatePlayhead();
}

void TNotesArea::setPlayPosition(double x) {
}
