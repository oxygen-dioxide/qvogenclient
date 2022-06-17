#include "TNotesArea.h"
#include "TNotesScroll.h"

void TNotesArea::updateSprite() {
    if (!m_spriteVisible || m_sprite->pixmap().isNull()) {
        if (m_sprite->isVisible()) {
            m_sprite->hide();
        }
        return;
    } else {
        m_sprite->show();
    }

    QSize size = m_sprite->pixmap().size();
    QRectF rect = m_view->viewportRect();

    double scale = rect.height() / size.height();
    if (scale * size.width() > rect.width()) {
        scale = rect.width() / size.width();
    }
    m_sprite->setScale(scale);

    QSizeF realSize = size * scale;

    switch (m_spritePosition) {
    case Qt::TopLeftCorner:
        m_sprite->setPos(rect.x(), rect.y());
        break;
    case Qt::TopRightCorner:
        m_sprite->setPos(rect.x() + rect.width() - realSize.width(), rect.y());
        break;
    case Qt::BottomLeftCorner:
        m_sprite->setPos(rect.x(), rect.y() + rect.height() - realSize.height());
        break;
    default:
        m_sprite->setPos(rect.x() + rect.width() - realSize.width(),
                         rect.y() + rect.height() - realSize.height());
        break;
    }
}

void TNotesArea::initSpriteElements() {
    m_spriteVisible = false;
    m_spritePosition = Qt::BottomRightCorner;

    m_sprite = new TNEdgeSprite(this);
    addItem(m_sprite);
    m_sprite->setZValue(spriteZIndex);

    m_sprite->setVisible(false);
}

void TNotesArea::loadSprite(const QString &path) {
    QPixmap bmp;
    if (bmp.load(path)) {
        m_sprite->setPixmap(bmp);
    } else {
        m_sprite->setPixmap(QPixmap());
    }
    updateSprite();
}

bool TNotesArea::spriteVisible() const {
    return m_spriteVisible;
}

void TNotesArea::setSpriteVisible(bool spriteVisible) {
    m_spriteVisible = spriteVisible;
    updateSprite();
}

Qt::Corner TNotesArea::spritePosition() const {
    return m_spritePosition;
}

void TNotesArea::setSpritePosition(const Qt::Corner &spritePosition) {
    m_spritePosition = spritePosition;
    updateSprite();
}

double TNotesArea::spriteAlpha() const {
    return m_sprite->opacity();
}

void TNotesArea::setSpriteAlpha(double spriteAlpha) {
    m_sprite->setOpacity(spriteAlpha);
}
