#include "TNSpriteCtl.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Events.h"

TNSpriteCtl::TNSpriteCtl(TNotesArea *parent) : TNController(parent) {
}

TNSpriteCtl::~TNSpriteCtl() {
}

void TNSpriteCtl::install() {
    m_spriteVisible = false;
    m_spritePosition = Qt::BottomRightCorner;

    a->installEventFilter(this);

    m_sprite = new TNEdgeSprite(a);
    a->addItem(m_sprite);
    m_sprite->setZValue(TNotesArea::Sprite);

    m_sprite->setVisible(false);
}

bool TNSpriteCtl::spriteVisible() const {
    return m_spriteVisible;
}

void TNSpriteCtl::setSpriteVisible(bool spriteVisible) {
    m_spriteVisible = spriteVisible;
    updateSprite();
}

Qt::Corner TNSpriteCtl::spritePosition() const {
    return m_spritePosition;
}

void TNSpriteCtl::setSpritePosition(const Qt::Corner &spritePosition) {
    m_spritePosition = spritePosition;
    updateSprite();
}

double TNSpriteCtl::spriteAlpha() const {
    return m_sprite->opacity();
}

void TNSpriteCtl::setSpriteAlpha(double spriteAlpha) {
    m_sprite->setOpacity(spriteAlpha);
}

void TNSpriteCtl::loadSprite(const QString &path) {
    QPixmap bmp;
    if (bmp.load(path)) {
        m_sprite->setPixmap(bmp);
    } else {
        m_sprite->setPixmap(QPixmap());
    }
    updateSprite();
}

void TNSpriteCtl::updateSprite() {
    if (!m_spriteVisible || m_sprite->pixmap().isNull()) {
        if (m_sprite->isVisible()) {
            m_sprite->hide();
        }
        return;
    } else {
        m_sprite->show();
    }

    QSize size = m_sprite->pixmap().size();
    QRectF rect = a->view()->viewportRect();

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

bool TNSpriteCtl::eventFilter(QObject *obj, QEvent *event) {
    if (obj == a) {
        switch (event->type()) {
        case QEvent::GraphicsSceneMove:
        case QEvent::GraphicsSceneResize:
            updateSprite();
            break;
        case QEventImpl::SceneStateChange: {
            auto e = static_cast<QEventImpl::SceneStateChangeEvent *>(event);
            // switch type 2
            switch (e->cType()) {
            case QEventImpl::SceneStateChangeEvent::SceneRect:
                updateSprite();
                break;
            default:
                break;
            }
            // end switch type 2

            break;
        }
        default:
            break;
        }
    }
    return TNController::eventFilter(obj, event);
}
