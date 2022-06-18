#include "TNNotesCtl.h"
#include "../Elements/TNRectNote.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Events.h"

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

bool TNNotesCtl::eventFilter(QObject *obj, QEvent *event) {
    if (obj == a) {
        switch (event->type()) {

        case QEventImpl::SceneRectChange: {
            auto e = static_cast<QEventImpl::SceneRectChangeEvent *>(event);
            if (e->sizeChanged()) {
                adjustGeometries();
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
