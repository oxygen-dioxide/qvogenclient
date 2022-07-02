#include "TNPlayheadCtl.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Events.h"

#include "Logs/CRecordHolder.h"

TNPlayheadCtl::TNPlayheadCtl(TNotesArea *parent) : TNController(parent) {
}

TNPlayheadCtl::~TNPlayheadCtl() {
}

void TNPlayheadCtl::install() {
    m_playToTick = 0;

    a->installEventFilter(this);

    m_playhead = new TNPlayhead(a);
    a->addItem(m_playhead);
    m_playhead->setZValue(TNotesArea::Playhead);

    m_playhead->setPos(0, 0);
    m_playhead->setAlwaysShow(true);
    m_playhead->setPlaying(false);

    updatePlayhead();
}

TNPlayhead *TNPlayheadCtl::playhead() const {
    return m_playhead;
}

void TNPlayheadCtl::setPlaying(bool visible) {
    m_playhead->setPlaying(visible);
}

bool TNPlayheadCtl::isPlaying() const {
    return m_playhead->playing();
}

void TNPlayheadCtl::setCurrentTick(int tick, bool adjust) {
    m_playToTick = tick;
    updatePlayhead();
    if (adjust) {
        QRectF vp = a->view()->viewportRect();
        if (qRecordCData.playheadState == 1) {
            a->setVisionFitToItem(m_playhead, Qt::AnchorHorizontalCenter);
        } else {
            if (m_playhead->left() < vp.left() || m_playhead->right() > vp.right()) {
                a->setVisionFitToItem(m_playhead, Qt::AnchorLeft);
            }
        }
    }
}

int TNPlayheadCtl::currentTick() const {
    return m_playToTick;
}

void TNPlayheadCtl::updatePlayhead() {
    if (m_playhead->rect().height() != a->sceneRect().height()) {
        m_playhead->setRect(-1, 0, 2, a->sceneRect().height());
    }
    m_playhead->setPos(double(m_playToTick) / 480 * a->currentWidth() + a->zeroLine(), 0);
}

bool TNPlayheadCtl::eventFilter(QObject *obj, QEvent *event) {
    if (obj == a) {
        switch (event->type()) {
        case QEvent::GraphicsSceneMove:
        case QEvent::GraphicsSceneResize:
            updatePlayhead();
            break;
        case QEventImpl::SceneStateChange: {
            auto e = static_cast<QEventImpl::SceneStateChangeEvent *>(event);
            // switch type 2
            switch (e->cType()) {
            case QEventImpl::SceneStateChangeEvent::SceneRect:
                updatePlayhead();
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
