#include "Elements/TNRubber.h"
#include "TNotesArea.h"
#include "TNotesScroll.h"

#include <QApplication>
#include <QDebug>
#include <QFocusEvent>
#include <QGraphicsSceneMouseEvent>

#include "../Utils/Events/SceneStateChange/TSSCSceneRectEvent.h"
#include "../Utils/Events/SceneStateQuery/TSSQCursorModeEvent.h"
#include "../Utils/Events/TDigitTimeSigEvent.h"
#include "../Utils/Events/TOperateEvent.h"

#include "../Utils/Operations/TOTempoTimeSig.h"

#include "Logs/CRecordHolder.h"

void TNotesArea::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    CGraphicsScene::mousePressEvent(event);
}

void TNotesArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    // Ensure event triggered by real mouse moving
    if (event->buttons()) {
        // First movement after move
        if (!m_moving) {
            CGraphicsScene::mouseMoveEvent(event);
            m_moving = true;

            QApplication::sendEvent(this, event);
            return;
        }
    }
    CGraphicsScene::mouseMoveEvent(event);
}

void TNotesArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    m_moving = false;
    CGraphicsScene::mouseReleaseEvent(event);
}

void TNotesArea::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    CGraphicsScene::mouseDoubleClickEvent(event);
}

void TNotesArea::focusOutEvent(QFocusEvent *event) {
    m_moving = false;
    CGraphicsScene::focusOutEvent(event);
}

void TNotesArea::customEvent(QEvent *event) {
    switch (event->type()) {
    case QEventImpl::SceneStateChange: {
        // Send a copy to window
        qApp->sendEvent(view()->window(), event);
        break;
    }
    case QEventImpl::SceneStateQuery: {
        auto e = static_cast<QEventImpl::SceneStateQueryEvent *>(event);
        // switch type 2
        switch (e->cType()) {
        case QEventImpl::SceneStateQueryEvent::CursorMode: {
            auto e2 = static_cast<TSSQCursorModeEvent *>(event);
            e2->mode = m_drawMode;
            break;
        }
        default:
            break;
        }
        // end switch type 2

        break;
    }
    case QEventImpl::SceneAction: {
        auto e = static_cast<QEventImpl::SceneActionEvent *>(event);
        // switch type 2
        switch (e->aType()) {
        case QEventImpl::SceneActionEvent::Digital: {
            auto e2 = static_cast<TDigitalEvent *>(e);
            // switch type 3
            switch (e2->dType()) {
            case TDigitalEvent::TimeSig: {
                auto e3 = static_cast<TDigitTimeSigEvent *>(e2);
                auto oldTimeSig = m_timeSig;
                setTimeSig(e3->at, e3->b);

                if (oldTimeSig != m_timeSig) {
                    auto op = new TOTempoTimeSig();
                    op->val = TOTempoTimeSig::Data{m_tempo, m_timeSig};
                    op->oldVal = TOTempoTimeSig::Data{m_tempo, oldTimeSig};
                    TOperateEvent oe;
                    oe.setData(op);
                    oe.dispatch(this);
                }
                break;
            }
            case TDigitalEvent::Tempo: {
                auto oldTempo = m_tempo;
                setTempo(e2->digitF);

                if (oldTempo != m_tempo) {
                    auto op = new TOTempoTimeSig();
                    op->val = TOTempoTimeSig::Data{m_tempo, m_timeSig};
                    op->oldVal = TOTempoTimeSig::Data{oldTempo, m_timeSig};
                    TOperateEvent oe;
                    oe.setData(op);
                    oe.dispatch(this);

                    removeAllCache();
                }
                break;
            }
            default:
                break;
            }
            // end switch type 3

            break;
        }
        case QEventImpl::SceneActionEvent::Append: {
            break;
        }
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

void TNotesArea::timerEvent(QTimerEvent *event) {
    if (event->timerId() == m_playerTimerId) {
        int tick = timeToTick(m_player->pos());
        if (tick > sectionCount() * 4 * 480) {
            stop();
        } else {
            m_playCtl->setCurrentTick(tick);
        }
    }
}

void TNotesArea::_q_sceneRectChanged(const QRectF &rect) {
    Q_UNUSED(rect)

    auto curSizes = qMakePair(QSize(currentWidth(), currentHeight()), sectionCount());
    auto oldSizes = this->m_oldSizes;

    m_oldSizes = curSizes;

    TSSCSceneRectEvent e;
    e.size = QSize(currentWidth(), currentHeight());
    e.oldSize = oldSizes.first;
    e.sections = sectionCount();
    e.oldSections = oldSizes.second;

    // Send event to this and window
    QApplication::sendEvent(this, &e);

    adjustBackground();
}

void TNotesArea::_q_eStateCommited(int eType) {
    switch (eType) {
    case CRecordHolder::Quantization: {
        setCurrentQuantize(qRecordCData.currentQuantize);
        break;
    }
    default:
        break;
    }
}
