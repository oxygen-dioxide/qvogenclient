#include "TNNotesCtl.h"
#include "../Elements/TNRectNote.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Events.h"
#include "Types/Graphics.h"

#include "../../Utils/Events/TOperateEvent.h"
#include "../../Utils/Operations/TONoteMove.h"
#include "../../Utils/Operations/TONoteStretch.h"

#include "ViewHelper.h"

#include <QApplication>

static const char MAIN_GROUP_NAME[] = "%MAIN%";

TNNotesCtl::TNNotesCtl(TNotesArea *parent) : TNController(parent) {
    m_timeBounds = new TNNoteList(this);
    m_selection = new TNNoteList(this);

    m_mainGroup = new TNNoteGroup(this);
    m_currentGroup = m_mainGroup;

    m_noteMaxId = 0;
}

TNNotesCtl::~TNNotesCtl() {
}

void TNNotesCtl::install() {
    a->installEventFilter(this);
}

void TNNotesCtl::setUtterances(const QList<TWProject::Utterance> &utters) {
    for (const auto &utter : utters) {
        auto g = (utter.name == MAIN_GROUP_NAME) ? m_mainGroup : new TNNoteGroup(this);

        g->name = utter.name;
        g->singer = utter.singer;
        g->rom = utter.romScheme;

        const auto &notes = utter.notes;
        for (const auto &note : notes) {
            auto p = createNote();

            p->start = note.start;
            p->length = note.length;
            p->tone = note.noteNum;
            p->lyric = note.rom;
            p->group = g;

            p->setEnabled(g == m_currentGroup);

            // Insert to group
            g->insert(p);

            // Insert to all notes
            m_timeBounds->insert(p);
            m_noteMap.insert(p->id, p);
        }

        // Insert to groups
        if (g != m_mainGroup) {
            m_noteGroups.append(g);
        }
    }

    adjustAllGeometry();
    adjustCanvas();

    // Move scroll
    {
        const auto &starts = m_timeBounds->begins();
        if (!starts.isEmpty()) {
            const auto &set = starts.front().second;
            auto p = qobject_cast<TNRectNote *>(*set.begin());
            a->setVisionFitToItem(p, Qt::AnchorVerticalCenter, false);
            a->setVisionFitToItem(p, Qt::AnchorHorizontalCenter, false);
        }
    }
}

void TNNotesCtl::moveNotes(const QList<TWNote::Movement> &moves) {
    for (const auto &move : qAsConst(moves)) {
        auto it = m_noteMap.find(move.id);
        if (it == m_noteMap.end()) {
            return;
        }
        auto note = it.value();
        note->start += move.hMove;
        note->tone += move.vMove;
        adjustGeometry(note);
    }
    adjustCanvas();
}

void TNNotesCtl::stretchNotes(const QList<TWNote::Stretch> &stretches) {
    for (const auto &stretch : qAsConst(stretches)) {
        auto it = m_noteMap.find(stretch.id);
        if (it == m_noteMap.end()) {
            return;
        }
        auto note = it.value();
        note->length += stretch.hStretch;
        adjustGeometry(note);
    }
    adjustCanvas();
}

void TNNotesCtl::selectAll() {
    setGroupSelected(m_currentGroup, true);
}

void TNNotesCtl::deselect() {
    const auto &selection = m_selection->begins();
    for (const auto &pair : selection) {
        const auto &set = pair.second;
        for (auto note : set) {
            note->setSelected(false);
        }
    }
    m_selection->clear();
}

void TNNotesCtl::switchGroup(TNNoteGroup *group) {
    if (m_currentGroup == group) {
        return;
    }
    m_currentGroup = group;
    for (auto &g : m_noteGroups) {
        if (g != m_currentGroup) {
            setGroupSelected(g, false);
            setGroupEnabled(g, false);
        }
    }
    setGroupEnabled(m_currentGroup, true);
}

bool TNNotesCtl::isMoving() const {
    return !m_movingData.isEmpty();
}

bool TNNotesCtl::isStretching() const {
    return !m_stretchingData.isEmpty();
}

TNRectNote *TNNotesCtl::createNote(quint64 id) {
    auto p = new TNRectNote(a);
    p->id = (id == 0) ? (++m_noteMaxId) : id;

    a->addItem(p);
    p->setZValue(a->Note);

    return p;
}

void TNNotesCtl::adjustGeometry(TNRectNote *note) {
    QEvent e(static_cast<QEvent::Type>(QEventImpl::ItemGeometryUpdate));
    QApplication::sendEvent(note, &e);
}

void TNNotesCtl::adjustGroupGeometry(const TNNoteGroup *group) {
    QList<TNRectNote *> notes;
    const auto &all = group->begins();
    for (const auto &pair : all) {
        const auto &set = pair.second;
        for (auto note : set) {
            notes.append(note);
        }
    }
    for (auto note : notes) {
        adjustGeometry(note);
    }
}

void TNNotesCtl::adjustAllGeometry() {
    QList<TNRectNote *> notes;
    const auto &all = m_timeBounds->begins();
    for (const auto &pair : all) {
        const auto &set = pair.second;
        for (auto note : set) {
            notes.append(note);
        }
    }
    for (auto note : notes) {
        adjustGeometry(note);
    }
}

void TNNotesCtl::adjustCanvas() {
    a->setSectionCountHint(totalLength());
}

void TNNotesCtl::selectOne(TNRectNote *p) {
    if (!p->isSelected() && p->isEnabled()) {
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

void TNNotesCtl::setGroupSelected(TNNoteGroup *group, bool selected) {
    const auto &all = group->begins();
    for (const auto &pair : all) {
        const auto &set = pair.second;
        for (auto note : set) {
            selected ? selectOne(note) : deselectOne(note);
        }
    }
}

void TNNotesCtl::setGroupEnabled(TNNoteGroup *group, bool enabled) {
    const auto &all = group->begins();
    for (const auto &pair : all) {
        const auto &set = pair.second;
        for (auto note : set) {
            note->setEnabled(enabled);
        }
    }
}

bool TNNotesCtl::eventFilter(QObject *obj, QEvent *event) {
    if (obj == a) {
        switch (event->type()) {

        // Mouse Press Event
        case QEvent::GraphicsSceneMousePress: {
            auto e = static_cast<QGraphicsSceneMouseEvent *>(event);
            auto item = a->itemUnderMouse();
            auto modifiers = qApp->keyboardModifiers();
            const auto &data = a->view()->controlData();
            if (item && item->type() == GraphicsImpl::NoteItem) {
                auto noteItem = static_cast<TNRectNote *>(item);
                // Get Next Behavior of Mouse Press
                TNRectSelectable::Behavior behavior = noteItem->mousePressBehavior();
                bool toMove = false;
                bool toStretch = false;
                switch (behavior) {
                case TNRectSelectable::SelectOne: {
                    selectOne(noteItem);
                    toMove = true;
                    break;
                }
                case TNRectSelectable::SelectOnly: {
                    deselect();
                    selectOne(noteItem);
                    toMove = true;
                    break;
                }
                case TNRectSelectable::SelectContinuously: {
                    selectOne(noteItem);
                    const auto &starts = m_selection->begins();

                    auto startItem = *starts.front().second.begin();
                    auto endItem = *starts.back().second.begin();

                    int startIndex = m_currentGroup->findBegin(startItem);
                    int endIndex = m_currentGroup->findBegin(endItem);

                    const auto &allStarts = m_currentGroup->begins();
                    for (int i = startIndex; i <= endIndex; ++i) {
                        const auto &set = allStarts.at(i).second;
                        for (auto note : set) {
                            selectOne(note);
                        }
                    }
                    toMove = true;
                    break;
                }
                case TNRectSelectable::DeselectOne: {
                    deselectOne(noteItem);
                    break;
                }
                case TNRectSelectable::IndependentStretch:
                case TNRectSelectable::RelativeStretch:
                case TNRectSelectable::AbsoluteStretch: {
                    if (!noteItem->isSelected()) {
                        deselect();
                        selectOne(noteItem);
                    }
                    toStretch = true;
                    break;
                }
                default: {
                    toMove = true;
                    break;
                }
                }
                if (toMove) {
                    // Start Movement
                    const auto &selection = m_selection->begins();
                    for (const auto &pair : selection) {
                        const auto &set = pair.second;
                        for (auto note : set) {
                            if (note->isEnabled()) {
                                m_movingData.append(MovingData{note, note->pos(), 0, 0});
                                m_startPoint = e->scenePos();
                            }
                        }
                    }
                } else if (toStretch) {
                    // Start Stretch
                    const auto &selection = m_selection->begins();
                    for (const auto &pair : selection) {
                        const auto &set = pair.second;
                        for (auto note : set) {
                            if (note->isEnabled()) {
                                m_stretchingData.append(
                                    StretchingData{note, note->pos(), note->size(), 0});
                                m_startPoint = e->scenePos();
                            }
                        }
                    }
                }
            } else if (modifiers != data.selectS) {
                deselect();
            }
            break;
        }

        // Mouse Move Event
        case QEvent::GraphicsSceneMouseMove: {
            auto e = static_cast<QGraphicsSceneMouseEvent *>(event);
            QPointF offset = e->scenePos() - m_startPoint;

            int fx = offset.x() < 0 ? -1 : 1;
            int fy = offset.y() < 0 ? -1 : 1;

            int q = a->currentQuantize();
            double w = double(a->currentWidth()) / q;
            double h = a->currentHeight();
            int tw = 480 / q;

            int dx = offset.x() / w + fx * 0.5;
            int dy = offset.y() / h + fy * 0.5;

            offset.setX(dx * w);
            offset.setY(dy * h);

            if (!m_movingData.isEmpty()) {
                for (auto &m : m_movingData) {
                    auto note = m.note;

                    int dx2 = dx * tw;
                    dx2 = int((note->start + dx2) / tw) * tw - note->start;
                    dx2 = qMax(0, note->start + dx2) - note->start;

                    int dy2 = -dy;
                    dy2 = qMin(qMax(24, note->tone + dy2), 107) - note->tone;

                    note->setPos(a->convertValueToPosition(note->start + dx2, note->tone + dy2));
                    m.dx = dx2;
                    m.dy = dy2;
                }
            } else if (!m_stretchingData.isEmpty()) {
                for (auto &s : m_stretchingData) {
                    auto note = s.note;
                    int end = note->start + note->length;

                    int dw = dx * tw;
                    dw = int((end + dw) / tw) * tw - end;
                    dw = qMax(-(note->length - 15), dw);

                    note->setSize(double(note->length + dw) / 480 * a->currentWidth(),
                                  note->height());
                    s.dw = dw;
                }
            }
            break;
        }

        // Mouse Release Event
        case QEvent::GraphicsSceneMouseRelease: {

            if (!m_movingData.isEmpty()) {
                QList<TONoteMove::MoveData> moves;
                for (const auto &m : qAsConst(m_movingData)) {
                    auto note = m.note;

                    // Update note
                    note->start += m.dx;
                    note->tone += m.dy;
                    adjustGeometry(note);

                    // Save movement
                    if (m.dx != 0 || m.dy != 0) {
                        moves.append(TONoteMove::MoveData{note->id, m.dx, m.dy});
                    }
                }
                m_movingData.clear();
                adjustCanvas();

                if (!moves.isEmpty()) {
                    // New Operation
                    TONoteMove *op = new TONoteMove();
                    op->moves = std::move(moves);

                    // Dispatch
                    TOperateEvent e;
                    e.setData(op);
                    e.dispatch(a);
                }
            } else if (!m_stretchingData.isEmpty()) {
                QList<TONoteStretch::StretchData> stretches;
                for (const auto &s : qAsConst(m_stretchingData)) {
                    auto note = s.note;

                    // Update note
                    note->length += s.dw;
                    adjustGeometry(note);

                    // Save stretch
                    if (s.dw != 0) {
                        stretches.append(TONoteStretch::StretchData{note->id, s.dw});
                    }
                }
                m_stretchingData.clear();
                adjustCanvas();

                if (!stretches.isEmpty()) {
                    // New Operation
                    TONoteStretch *op = new TONoteStretch();
                    op->stretches = std::move(stretches);

                    // Dispatch
                    TOperateEvent e;
                    e.setData(op);
                    e.dispatch(a);
                }
            }
            break;
        }

        // Mouse Double Click Event
        case QEvent::GraphicsSceneMouseDoubleClick: {
            auto item = a->itemUnderMouse();
            if (item && item->type() == GraphicsImpl::NoteItem) {
                auto noteItem = static_cast<TNRectNote *>(item);
                switchGroup(noteItem->group);
            }
            break;
        }


        case QEventImpl::SceneRectChange: {
            auto e = static_cast<QEventImpl::SceneRectChangeEvent *>(event);
            if (e->sizeChanged()) {
                adjustAllGeometry();
            }
            break;
        }

        case QEventImpl::SceneRubberSelect: {
            auto e = static_cast<QEventImpl::SceneRubberSelectEvent *>(event);
            const auto &all = m_currentGroup->begins();
            for (const auto &pair : all) {
                const auto &set = pair.second;
                for (auto note : set) {
                    if (!note->isSelected() &&
                        View::rectHitTest(QRectF(note->pos(), note->rect().size()), e->rect())) {
                        selectOne(note);
                    }
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
    const auto &set = starts.front().second;
    auto p = qobject_cast<TNRectNote *>(*set.begin());
    return p->start;
}

int TNNotesCtl::totalLength() const {
    const auto &ends = m_timeBounds->ends();
    if (ends.isEmpty()) {
        return 0;
    }
    const auto &set = ends.back().second;
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
