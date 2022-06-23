#include "TNotesArea.h"
#include "TNotesScroll.h"

void TNotesArea::loadSprite(const QString &path) {
    m_spriteCtl->loadSprite(path);
}

bool TNotesArea::spriteVisible() const {
    return m_spriteCtl->spriteVisible();
}

void TNotesArea::setSpriteVisible(bool spriteVisible) {
    m_spriteCtl->setSpriteVisible(spriteVisible);
}

Qt::Corner TNotesArea::spritePosition() const {
    return m_spriteCtl->spritePosition();
}

void TNotesArea::setSpritePosition(const Qt::Corner &spritePosition) {
    m_spriteCtl->setSpritePosition(spritePosition);
}

double TNotesArea::spriteAlpha() const {
    return m_spriteCtl->spriteAlpha();
}

void TNotesArea::setSpriteAlpha(double spriteAlpha) {
    m_spriteCtl->setSpriteAlpha(spriteAlpha);
}

void TNotesArea::setProjectData(const TWrappedData &data) {
    m_notesCtl->setUtterances(data.utterances);
}
