#include "TNPlayheadCtl.h"
#include "../TNotesArea.h"

#include "Types/Events.h"

TNPlayheadCtl::TNPlayheadCtl(TNotesArea *parent) : TNController(parent) {
}

TNPlayheadCtl::~TNPlayheadCtl() {
}

void TNPlayheadCtl::install() {
    m_playToNote = 0;
    m_playToPosition = 0;
    m_playToTick = 0;

    a->installEventFilter(this);

    m_playhead = new TNPlayhead(a);
    a->addItem(m_playhead);
    m_playhead->setZValue(TNotesArea::Playhead);

    m_playhead->setPos(0, 0);
    m_playhead->setAlwaysShow(false);
    m_playhead->setPlaying(false);

    updatePlayhead();
}

void TNPlayheadCtl::updatePlayhead() {
    if (m_playhead->rect().height() != a->sceneRect().height()) {
        m_playhead->setRect(-1, 0, 2, a->sceneRect().height());
    }
    m_playhead->setPos(m_playToTick / 480 * a->currentWidth() + a->zeroLine(), 0);
}

bool TNPlayheadCtl::eventFilter(QObject *obj, QEvent *event) {
    if (obj == a) {
        switch (event->type()) {

        case QEvent::GraphicsSceneMove:
        case QEvent::GraphicsSceneResize:
        case QEventImpl::SceneRectChange:
            updatePlayhead();
            break;

        default:
            break;
        }
    }
    return TNController::eventFilter(obj, event);
}
