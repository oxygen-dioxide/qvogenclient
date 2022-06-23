#include "TNNotesCtl.h"
#include "../Elements/TNRectNote.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Events.h"
#include "Types/Graphics.h"

#include "ViewHelper.h"

#include <QApplication>

static const char MAIN_GROUP_NAME[] = "%MAIN%";

TNNotesCtl::TNNotesCtl(TNotesArea *parent) : TNController(parent) {
    m_timeBounds = new TNNoteList(this);
    m_selection = new TNNoteList(this);

    m_mainGroup = new TNNoteGroup(this);
    m_currentGroup = m_mainGroup;
}

TNNotesCtl::~TNNotesCtl() {
}

void TNNotesCtl::install() {
    a->installEventFilter(this);
}

void TNNotesCtl::setUtterances(const QList<TWrappedData::Utterance> &utters) {
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
            const auto &set = starts.front();
            auto p = qobject_cast<TNRectNote *>(*set.begin());
            a->setVisionFitToItem(p, Qt::AnchorVerticalCenter, false);
            a->setVisionFitToItem(p, Qt::AnchorHorizontalCenter, false);
        }
    }
}

void TNNotesCtl::selectAll() {
    setGroupSelected(m_currentGroup, true);
}

void TNNotesCtl::deselect() {
    const auto &selection = m_selection->begins();
    for (const auto &set : selection) {
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

void TNNotesCtl::adjustGroupGeometry(const TNNoteGroup *group) {
    QList<TNRectNote *> notes;
    const auto &sets = group->begins();
    for (const auto &set : sets) {
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
    const auto &sets = m_timeBounds->begins();
    for (const auto &set : sets) {
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
    for (const auto &set : all) {
        for (auto note : set) {
            selected ? selectOne(note) : deselectOne(note);
        }
    }
}

void TNNotesCtl::setGroupEnabled(TNNoteGroup *group, bool enabled) {
    const auto &all = group->begins();
    for (const auto &set : all) {
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

                    auto startItem = qobject_cast<TNRectNote *>(*starts.front().begin());
                    auto endItem = qobject_cast<TNRectNote *>(*starts.back().begin());

                    int startIndex = m_currentGroup->findBegin(startItem);
                    int endIndex = m_currentGroup->findBegin(endItem);

                    const auto &allStarts = m_currentGroup->begins();
                    for (int i = startIndex; i <= endIndex; ++i) {
                        const auto &set = allStarts.at(i);
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
                    selectOne(noteItem);
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
                    for (const auto &set : selection) {
                        for (auto note : set) {
                            if (note->isEnabled()) {
                                MovingData m;
                                m.note = note;
                                m.originPos = note->pos();
                                m_movingData.append(m);
                                m.dx = 0;
                                m.dy = 0;

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

            double w = double(a->currentWidth()) / a->currentQuantize();
            double h = a->currentHeight();

            int dx = int(offset.x() / w + fx * 0.5);
            int dy = int(offset.y() / h + fy * 0.5);

            offset.setX(dx * w);
            offset.setY(dy * h);

            // Movement
            if (!m_movingData.isEmpty()) {
                for (auto &m : m_movingData) {
                    m.note->setPos(m.originPos + offset);
                    m.dx = dx;
                    m.dy = dy;
                }
            }
            break;
        }

        // Mouse Release Event
        case QEvent::GraphicsSceneMouseRelease: {

            // Movement
            if (!m_movingData.isEmpty()) {
                for (const auto &m : m_movingData) {
                    adjustGeometry(m.note);
                }
                m_movingData.clear();
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
            const auto &sets = m_currentGroup->begins();
            for (const auto &set : sets) {
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
