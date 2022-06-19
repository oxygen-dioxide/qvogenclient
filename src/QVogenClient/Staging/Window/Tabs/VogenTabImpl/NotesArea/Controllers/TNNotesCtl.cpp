#include "TNNotesCtl.h"
#include "../Elements/TNRectNote.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Events.h"
#include "Types/Graphics.h"

#include "ViewHelper.h"

#include <QApplication>

TNNotesCtl::TNNotesCtl(TNotesArea *parent) : TNController(parent) {
    m_timeBounds = new TNEntityList(this);
    m_selection = new TNEntityList(this);
}

TNNotesCtl::~TNNotesCtl() {
}

void TNNotesCtl::install() {
    a->installEventFilter(this);
}

void TNNotesCtl::setNotesFromCommon(const QList<CommonNote> &notes) {
    const QSignalBlocker guard(m_timeBounds);

    for (auto it = notes.begin(); it != notes.end(); ++it) {
        const auto &note = *it;
        auto p = createNote();

        p->start = note.start;
        p->length = note.length;
        p->tone = note.noteNum;
        p->lyric = note.lyric;

        // Insert to note management
        m_notes.append(p);
        m_timeBounds->insert(p);
    }

    adjustGeometries();

    adjustCanvas();
}

const QList<TNRectNote *> &TNNotesCtl::notes() const {
    return m_notes;
}

void TNNotesCtl::selectAll() {
    for (auto it = m_notes.begin(); it != m_notes.end(); ++it) {
        const auto &note = *it;
        selectOne(note);
    }
}

void TNNotesCtl::deselect() {
    const auto &selection = m_selection->begins();
    for (auto it = selection.begin(); it != selection.end(); ++it) {
        const auto &set = *it;
        for (auto it2 = set.begin(); it2 != set.end(); ++it2) {
            (*it2)->setSelected(false);
        }
    }
    m_selection->clear();
}

TNRectNote *TNNotesCtl::createNote() {
    auto p = new TNRectNote(a);
    a->addItem(p);
    p->setZValue(a->Note);
    return p;
}

void TNNotesCtl::adjustGeometry(TNRectNote *note) {
    QEvent e(static_cast<QEvent::Type>(QEventImpl::ItemGeometryUpdate));
    QApplication::sendEvent(note, &e);
}

void TNNotesCtl::adjustGeometries() {
    for (auto it = m_notes.begin(); it != m_notes.end(); ++it) {
        adjustGeometry(*it);
    }
}

void TNNotesCtl::adjustCanvas() {
    a->setSectionCountHint(totalLength());
}

void TNNotesCtl::selectOne(TNRectNote *p) {
    if (!p->isSelected()) {
        p->setSelected(true);
        m_selection->insert(p);
    }
}

void TNNotesCtl::deselectOne(TNRectNote *p) {
    if (p->isSelected()) {
        p->setSelected(false);
        m_selection->remove(p);
    }
}

bool TNNotesCtl::eventFilter(QObject *obj, QEvent *event) {
    if (obj == a) {
        switch (event->type()) {

        // Mouse Press Event
        case QEvent::GraphicsSceneMousePress: {
            auto item = a->itemUnderMouse();
            auto modifiers = qApp->keyboardModifiers();
            const auto &data = a->view()->controlData();
            if (item && item->type() == GraphicsImpl::NoteItem) {
                auto noteItem = static_cast<TNRectNote *>(item);
                TNRectSelectable::Behavior res = noteItem->mousePressBehavior();
                switch (res) {
                case TNRectSelectable::SelectOne: {
                    selectOne(noteItem);
                    break;
                }
                case TNRectSelectable::SelectOnly: {
                    deselect();
                    selectOne(noteItem);
                    break;
                }
                case TNRectSelectable::SelectContinuously: {
                    selectOne(noteItem);
                    const auto &starts = m_selection->begins();

                    auto startItem = qobject_cast<TNRectNote *>(*starts.front().begin());
                    auto endItem = qobject_cast<TNRectNote *>(*starts.back().begin());

                    int startIndex = m_timeBounds->findBegin(startItem);
                    int endIndex = m_timeBounds->findBegin(endItem);

                    const auto &allStarts = m_timeBounds->begins();
                    for (int i = startIndex; i <= endIndex; ++i) {
                        const auto &set = allStarts.at(i);
                        for (auto it2 = set.begin(); it2 != set.end(); ++it2) {
                            auto item = qobject_cast<TNRectNote *>(*it2);
                            selectOne(item);
                        }
                    }
                    break;
                }
                case TNRectSelectable::DeselectOne: {
                    deselectOne(noteItem);
                    break;
                }
                case TNRectSelectable::IndependentStretch: {
                    break;
                }
                case TNRectSelectable::RelativeStretch: {
                    break;
                }
                case TNRectSelectable::AbsoluteStretch: {
                    break;
                }
                default:
                    break;
                }
            } else if (modifiers != data.selectS) {
                deselect();
            }
            break;
        }

        case QEvent::GraphicsSceneMouseMove: {
            break;
        }

        case QEvent::GraphicsSceneMouseRelease: {
            break;
        }

        case QEventImpl::SceneRectChange: {
            auto e = static_cast<QEventImpl::SceneRectChangeEvent *>(event);
            if (e->sizeChanged()) {
                adjustGeometries();
            }
            break;
        }

        case QEventImpl::SceneRubberSelect: {
            auto e = static_cast<QEventImpl::SceneRubberSelectEvent *>(event);
            for (auto it = m_notes.begin(); it != m_notes.end(); ++it) {
                const auto &note = *it;
                if (!note->isSelected() &&
                    View::rectHitTest(QRectF(note->pos(), note->rect().size()), e->rect())) {
                    selectOne(note);
                }
            }
            break;
        }

        default:
            break;
        }
    }
    return TNController::eventFilter(obj, event);
}

int TNNotesCtl::startTick() const {
    const auto &starts = m_timeBounds->begins();
    if (starts.isEmpty()) {
        return 0;
    }
    const auto &set = starts.front();
    auto p = qobject_cast<TNRectNote *>(*set.begin());
    return p->start;
}

int TNNotesCtl::totalLength() const {
    const auto &ends = m_timeBounds->begins();
    if (ends.isEmpty()) {
        return 0;
    }
    const auto &set = ends.back();
    auto p = qobject_cast<TNRectNote *>(*set.begin());
    return p->start + p->length;
}

void TNNotesCtl::_q_beginChanged(int index, int val) {
    Q_UNUSED(index);
    Q_UNUSED(val);
}

void TNNotesCtl::_q_endChanged(int index, int val) {
    Q_UNUSED(val);
    if (index == m_timeBounds->ends().size() - 1) {
        adjustCanvas();
    }
}
