#include "TNNotesCtl.h"
#include "../Elements/TNRectNote.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Events.h"
#include "Types/Graphics.h"

#include "ViewHelper.h"

#include <QApplication>

TNNotesCtl::TNNotesCtl(TNotesArea *parent) : TNController(parent) {
}

TNNotesCtl::~TNNotesCtl() {
}

void TNNotesCtl::install() {
    a->installEventFilter(this);
}

void TNNotesCtl::setNotesFromCommon(const QList<CommonNote> &notes) {
    for (auto it = notes.begin(); it != notes.end(); ++it) {
        const auto &note = *it;
        auto p = createNote();

        p->start = note.start;
        p->length = note.length;
        p->tone = note.noteNum;
        p->lyric = note.lyric;

        m_notes.append(p);

        insertNoteRef(p);
    }
    adjustGeometries();
    a->setSectionCountHint(totalLength());
}

const QList<TNRectNote *> &TNNotesCtl::notes() const {
    return m_notes;
}

void TNNotesCtl::selectAll() {
    for (auto it = m_notes.begin(); it != m_notes.end(); ++it) {
        const auto &note = *it;
        if (!note->isSelected()) {
            selectOne(note);
        }
    }
}

void TNNotesCtl::deselect() {
    for (auto it = m_selection.begin(); it != m_selection.end(); ++it) {
        (*it)->setSelected(false);
    }
    m_selection.clear();
}

TNRectNote *TNNotesCtl::createNote() {
    auto p = new TNRectNote(a);
    a->addItem(p);
    p->setZValue(a->Note);
    return p;
}

void TNNotesCtl::adjustGeometries() {
    for (auto it = m_notes.begin(); it != m_notes.end(); ++it) {
        QEvent e(QEvent::LayoutRequest);
        QApplication::sendEvent(*it, &e);
    }
}

void TNNotesCtl::selectOne(TNRectNote *p) {
    p->setSelected(true);
    m_selection.insert(p);
}

void TNNotesCtl::deselectOne(TNRectNote *p) {
    p->setSelected(false);
    m_selection.remove(p);
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
                    if (m_selection.isEmpty()) {
                        selectOne(noteItem);
                    } else {
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

void TNNotesCtl::insertNoteRef(TNRectNote *p) {
    m_startRef.insert(p);
    m_endRef.insert(p);
}

void TNNotesCtl::removeNoteRef(TNRectNote *p) {
    m_startRef.remove(p);
    m_endRef.remove(p);
}

void TNNotesCtl::updateNoteRefs(const QList<TNRectNote *> &notes) {
    for (auto p : notes) {
        m_startRef.remove(p);
        m_endRef.remove(p);
    }
    for (auto p : notes) {
        m_startRef.insert(p);
        m_endRef.insert(p);
    }
}

int TNNotesCtl::startTick() const {
    if (m_startRef.isEmpty()) {
        return 0;
    }
    auto front = *m_startRef.begin();
    return front->start;
}

int TNNotesCtl::totalLength() const {
    if (m_endRef.isEmpty()) {
        return 0;
    }
    auto front = *m_endRef.rbegin();
    return front->start + front->length;
}

bool TNNotesCtl::NoteComparator_Start::operator()(TNRectNote *p1, const TNRectNote *p2) const {
    return p1->start < p2->start;
}

bool TNNotesCtl::NoteComparator_End::operator()(TNRectNote *p1, const TNRectNote *p2) const {
    return p1->start + p1->length < p2->start + p2->length;
}
