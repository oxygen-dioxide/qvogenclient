#include "TNNotesCtl.h"
#include "../Elements/TNRectNote.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Actions.h"
#include "Types/Events.h"
#include "Types/Graphics.h"

#include "../../Utils/Events/TDigitalEvent.h"
#include "../../Utils/Events/TOperateEvent.h"

#include "../../Utils/Operations/TOGroupChange.h"
#include "../../Utils/Operations/TOLyricsChange.h"
#include "../../Utils/Operations/TONoteInsDel.h"
#include "../../Utils/Operations/TONoteMove.h"
#include "../../Utils/Operations/TONoteStretch.h"

#include "MathHelper.h"
#include "ViewHelper.h"

#include "Namespace.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#include <QJsonDocument>

static const char MAIN_GROUP_NAME[] = "%MAIN%";

static const char LYRICS_SEPARATOR = ' ';

TNNotesCtl::TNNotesCtl(TNotesArea *parent) : TNController(parent) {
    m_timeBounds = new TNNoteList(this);
    m_selection = new TNNoteList(this);

    m_maxNoteId = 0;
    m_maxGroupId = 0;

    m_movedNoteIndex = -1;

    createGroup(0, MAIN_GROUP_NAME, QString(), QString()); // main group is set

    m_currentGroup = m_mainGroup;

    m_editing = false;
}

TNNotesCtl::~TNNotesCtl() {
}

void TNNotesCtl::install() {
    a->installEventFilter(this);
}

void TNNotesCtl::setUtterances(const QList<TWProject::Utterance> &utters) {
    for (const auto &utter : utters) {
        auto g = (utter.name == MAIN_GROUP_NAME)
                     ? m_mainGroup
                     : createGroup(0, utter.name, utter.singer, utter.romScheme);

        const auto &notes = utter.notes;
        for (const auto &note : notes) {
            auto p = createNote(0, note.start, note.length, note.noteNum, note.rom, g);
            adjustGeometry(p);
        }
    }

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

QList<TWProject::Utterance> TNNotesCtl::utterances() const {
    QList<TWProject::Utterance> utters;

    // Group List Temp
    QMap<QString, TNNoteGroup *> groups;
    groups.insert(m_mainGroup->name, m_mainGroup);
    for (const auto &g : m_noteGroups) {
        groups.insert(g->name, g);
    }

    for (const auto &group : groups) {
        if (group->isEmpty()) {
            continue;
        }

        TWProject::Utterance u;

        u.name = group->name;
        u.singer = group->singer;
        u.romScheme = group->rom;

        QList<TWProject::Note> notes;
        const auto &all = group->begins();
        for (const auto &pair : qAsConst(all)) {
            const auto &set = pair.second;
            for (auto note : set) {
                TWProject::Note n;
                n.start = note->start;
                n.length = note->length;
                n.noteNum = note->tone;
                n.rom = note->lyric;
                notes.append(n);
            }
        }

        u.notes = std::move(notes);
        utters.append(u);
    }
    return utters;
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

void TNNotesCtl::changeLyrics(const QList<TWNote::Lyric> &lyrics) {
    for (const auto &lrc : qAsConst(lyrics)) {
        auto it = m_noteMap.find(lrc.id);
        if (it == m_noteMap.end()) {
            return;
        }
        auto note = it.value();
        note->lyric = lrc.lyric;
        note->update();
    }
}

void TNNotesCtl::addNotes(const QList<TWNote::NoteAll> &notes) {
    for (const auto &note : qAsConst(notes)) {
        auto g = findGroup(note.gid);
        if (!g) {
            continue;
        }
        auto p = createNote(note.id, note.start, note.length, note.noteNum, note.lyric, g);
        adjustGeometry(p);
    }
    adjustCanvas();
}

void TNNotesCtl::removeNotes(const QList<quint64> &ids) {
    for (quint64 id : qAsConst(ids)) {
        auto it = m_noteMap.find(id);
        if (it == m_noteMap.end()) {
            continue;
        }
        auto p = it.value();
        removeNote(p);
    }
    adjustCanvas();
}

void TNNotesCtl::changeGroup(const QList<quint64> &ids, const TWNote::GroupAll &group) {
    auto g = findGroup(group.id);
    if (!g) {
        g = createGroup(group.id, group.name, group.singer, group.rom);
    }

    QSet<TNNoteGroup *> groups;
    for (quint64 id : qAsConst(ids)) {
        auto it = m_noteMap.find(id);
        if (it == m_noteMap.end()) {
            continue;
        }
        auto p = it.value();
        auto oldGroup = p->group;

        // Modify group
        changeNoteGroup(p, g);

        groups.insert(oldGroup);
    }

    switchGroup(g);

    for (auto group : qAsConst(groups)) {
        if (group != m_mainGroup && group->isEmpty()) {
            removeGroup(group);
        }
    }
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

    // Update Menu
    TPianoRollEvent e(TPianoRollEvent::Select);
    e.dispatch(a);
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
    if (m_mainGroup != m_currentGroup) {
        setGroupSelected(m_mainGroup, false);
        setGroupEnabled(m_mainGroup, false);
    }
    setGroupEnabled(m_currentGroup, true);
}

TNNoteGroup *TNNotesCtl::findGroup(quint64 gid) const {
    if (gid == m_mainGroup->id) {
        return m_mainGroup;
    }
    auto it = m_noteGroups.find(gid);
    if (it == m_noteGroups.end()) {
        return nullptr;
    }
    return it.value();
}

bool TNNotesCtl::isMoving() const {
    return !m_movingData.isEmpty();
}

bool TNNotesCtl::isStretching() const {
    return !m_stretchingData.isEmpty();
}

bool TNNotesCtl::isDrawing() const {
    return !m_drawingData.isEmpty();
}

bool TNNotesCtl::isLyricsEditing() const {
    return m_editing;
}

bool TNNotesCtl::hasSelection() const {
    return !m_selection->isEmpty();
}

quint64 TNNotesCtl::currentGroupId() const {
    return m_currentGroup->id;
}

TNRectNote *TNNotesCtl::createNote(quint64 id, int start, int len, int tone, const QString &lrc,
                                   TNNoteGroup *g) {
    auto p = new TNRectNote(a);

    p->id = (id == 0) ? (++m_maxNoteId) : id;

    p->start = start;
    p->length = len;
    p->tone = tone;
    p->lyric = lrc;
    p->group = g;

    p->setEnabled(g == m_currentGroup);

    // Insert to group
    g->insert(p);

    // Insert to all notes
    m_timeBounds->insert(p);
    m_noteMap.insert(p->id, p);

    // Add to scene
    a->addItem(p);
    p->setZValue(a->Note);

    return p;
}

void TNNotesCtl::removeNote(TNRectNote *p) {
    auto g = p->group;

    // Remove from selection if selected
    if (p->isSelected()) {
        deselectOne(p);
    }

    // Remove from group
    g->remove(p);

    // Remove from all notes
    m_timeBounds->remove(p);
    m_noteMap.remove(p->id);

    p->deleteLater();
}

void TNNotesCtl::changeNoteGroup(TNRectNote *p, TNNoteGroup *g) {
    p->group->remove(p);
    g->insert(p);
    p->group = g;
    p->setEnabled(g == m_currentGroup);
}

void TNNotesCtl::setNotesMovable(bool movable) {
    const auto &all = m_timeBounds->begins();
    for (const auto &pair : all) {
        const auto &set = pair.second;
        for (auto note : set) {
            note->setMovable(movable);
        }
    }
}

TNNoteGroup *TNNotesCtl::createGroup(quint64 id, const QString &name, const QString &singer,
                                     const QString &rom) {
    auto g = new TNNoteGroup(a, this);
    g->id = (id == 0) ? (++m_maxGroupId) : id;
    g->name = name.isEmpty() ? ("utt-" + QString::number(g->id)) : name;
    g->singer = singer;
    g->rom = rom;

    // Insert to groups
    if (g->id > 1) {
        m_noteGroups.insert(g->id, g);
    } else {
        m_mainGroup = g;
    }

    g->install();
    g->adjustHintGeometry();

    return g;
}

void TNNotesCtl::removeGroup(TNNoteGroup *g) {
    Q_ASSERT(g != m_mainGroup);

    g->uninstall();

    m_noteGroups.remove(g->id);
    g->deleteLater();
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

        // Update Menu
        TPianoRollEvent e(TPianoRollEvent::Select);
        e.dispatch(a);
    }
}

void TNNotesCtl::deselectOne(TNRectNote *p) {
    if (p->isSelected()) {
        p->setSelected(false);
        m_selection->remove(p);

        // Update Menu
        TPianoRollEvent e(TPianoRollEvent::Select);
        e.dispatch(a);
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

QList<TNRectNote *> TNNotesCtl::tryApplyLyrics(int len) {
    QList<TNRectNote *> res;
    TNRectNote *lastNote = nullptr;
    TNNoteGroup *g = nullptr;

    if (len == 0) {
        return res;
    }

    // Fill Selected Notes
    const auto &selection = m_selection->begins();
    for (const auto &pair : selection) {
        const auto &set = pair.second;
        auto note = *set.begin();
        if (!g) {
            g = note->group;
        }
        res.append(note);
        len--;
        lastNote = note;
        if (len == 0) {
            break;
        }
    }

    if (!g) {
        Q_ASSERT(0);
        return res;
    }

    // Fill Following Notes
    if (len > 0) {
        const auto &all = g->begins();
        int index = g->findBegin(lastNote);
        for (int i = index + 1; i < all.size(); ++i) {
            auto note = *all.at(i).second.begin();
            res.append(note);
            len--;
            if (len == 0) {
                break;
            }
        }
    }
    return res;
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
            if (a->drawMode() != TNotesArea::DrawCurves && item &&
                item->type() == GraphicsImpl::NoteItem) {
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
                if (e->button() == Qt::LeftButton) {
                    if (toMove) {
                        if (noteItem->movable()) {
                            // Start Movement
                            const auto &selection = m_selection->begins();
                            m_movedNoteIndex = -1;
                            for (const auto &pair : selection) {
                                const auto &set = pair.second;
                                for (auto note : set) {
                                    m_movingData.append(MovingData{note, note->pos(), 0, 0});
                                    if (note == noteItem) {
                                        m_movedNoteIndex = m_movingData.size() - 1;
                                    }
                                }
                            }
                            if (m_movedNoteIndex < 0) {
                                m_movingData.clear();
                            }
                        }
                    } else if (toStretch) {
                        // Start Stretch
                        const auto &selection = m_selection->begins();
                        for (const auto &pair : selection) {
                            const auto &set = pair.second;
                            for (auto note : set) {
                                m_stretchingData.append(
                                    StretchingData{note, note->pos(), note->size(), 0});
                            }
                        }
                    }
                }
            } else if (modifiers != data.selectS) {
                deselect();
            }
            m_startPoint = e->scenePos();
            break;
        }

        // Mouse Move Event
        case QEvent::GraphicsSceneMouseMove: {
            auto e = static_cast<QGraphicsSceneMouseEvent *>(event);
            auto item = a->itemUnderMouse();
            if (e->buttons() & Qt::LeftButton) {
                if (a->mouseMoving()) {
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
                        // Adjust delta
                        int dx1 = dx * tw;
                        int dy1 = -dy;
                        {
                            // Dominated By Central Note
                            {
                                auto &m = m_movingData[m_movedNoteIndex];
                                auto note = m.note;

                                // Adjust dx
                                dx1 = int((note->start + dx1) / tw) * tw - note->start;
                                dx1 = qMax(0, note->start + dx1) - note->start;

                                // Adjust dy
                                dy1 = qMin(qMax(24, note->tone + dy1), 107) - note->tone;
                            }

                            // Consider Other Notes
                            for (int i = 0; i < m_movingData.size(); ++i) {
                                if (i == m_movedNoteIndex) {
                                    continue;
                                }
                                auto &m = m_movingData[i];
                                auto note = m.note;

                                int dx2 = dx1;
                                dx2 = qMax(0, note->start + dx2) - note->start;

                                int dy2 = -dy;
                                dy2 = qMin(qMax(24, note->tone + dy2), 107) - note->tone;

                                dx1 = (dx1 < 0) ? qMax(dx1, dx2) : qMin(dx1, dx2);
                                dy1 = (dy1 < 0) ? qMax(dy1, dy2) : qMin(dy1, dy2);
                            }
                        }

                        // Modify
                        for (auto &m : m_movingData) {
                            auto note = m.note;
                            note->setPos(
                                a->convertValueToPosition(note->start + dx1, note->tone + dy1));
                            m.dx = dx1;
                            m.dy = dy1;
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
                    } else if (!m_drawingData.isEmpty()) {
                        auto &s = m_drawingData.front();
                        auto note = s.note;

                        int end = note->start + note->length;

                        int dw = dx * tw;
                        dw = int((end + dw) / tw) * tw - end;
                        dw = qMax(-(note->length - 15), dw);

                        note->setSize(double(note->length + dw) / 480 * a->currentWidth(),
                                      note->height());
                        s.dw = dw;
                    }
                } else {
                    if (!a->isSelecting() && !item && a->drawMode() == TNotesArea::DrawNote) {
                        // Start draw
                        if (m_startPoint.x() >= a->zeroLine()) {
                            int q = a->currentQuantize();
                            double h = a->currentHeight();
                            int tw = 480 / q;

                            auto val = a->convertPositionToValue(
                                QPointF(m_startPoint.x(), m_startPoint.y() - h));
                            auto val2 = a->convertPositionToValue(e->scenePos());

                            int tick1 = int(val.first / tw) * tw;
                            int tick2 = val2.first;
                            int len = tick2 - tick1;

                            auto newPoint = a->convertValueToPosition(tick1, val.second);
                            m_startPoint = newPoint;

                            auto p = createNote(0, tick1, len, val.second, "a", m_currentGroup);
                            adjustGeometry(p);

                            p->setPos(a->convertValueToPosition(tick1, val.second));
                            p->setSize(double(len) / 480 * a->currentWidth(), h);

                            m_drawingData.append(DrawingData{p, 0});
                        }
                    }
                }
            }
            break;
        }

        // Mouse Release Event
        case QEvent::GraphicsSceneMouseRelease: {

            auto e = static_cast<QGraphicsSceneMouseEvent *>(event);
            if (e->button() == Qt::LeftButton) {
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
                        auto op = new TONoteMove();
                        op->data = std::move(moves);

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
                        auto op = new TONoteStretch();
                        op->data = std::move(stretches);

                        // Dispatch
                        TOperateEvent e;
                        e.setData(op);
                        e.dispatch(a);
                    }
                } else if (!m_drawingData.isEmpty()) {
                    auto &s = m_drawingData.front();
                    auto note = s.note;

                    // Update note
                    note->length += s.dw;
                    adjustGeometry(note);

                    m_drawingData.clear();
                    adjustCanvas();

                    // Save new note
                    {
                        QList<TONoteInsDel::NoteData> notes{TONoteInsDel::NoteData{
                            note->id,        // Id
                            note->start,     // Start
                            note->length,    // Length
                            note->tone,      // Tone
                            note->lyric,     // Lyric
                            note->group->id, // Gid
                        }};

                        // New Operation
                        auto op = new TONoteInsDel(TONoteInsDel::Create);
                        op->data = std::move(notes);

                        // Dispatch
                        TOperateEvent e;
                        e.setData(op);
                        e.dispatch(a);
                    }
                }
            }
            break;
        }

        // Mouse Double Click Event
        case QEvent::GraphicsSceneMouseDoubleClick: {
            auto e = static_cast<QGraphicsSceneMouseEvent *>(event);
            if (e->button() == Qt::LeftButton) {
                auto item = a->itemUnderMouse();
                if (item) {
                    if (item->type() == GraphicsImpl::NoteItem) {
                        auto noteItem = static_cast<TNRectNote *>(item);
                        switchGroup(noteItem->group);
                    }
                } else {
                    switchGroup(m_mainGroup);
                }
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

        case QEventImpl::StdinRequest: {
            auto e = static_cast<QEventImpl::StdinRequestEvent *>(event);
            switch (e->iType()) {
            case QEventImpl::StdinRequestEvent::Lyrics: {
                // Handle Lyrics Input
                switch (e->iProcess()) {
                case QEventImpl::StdinRequestEvent::InputStart: {
                    if (m_selection->isEmpty() || !m_selection->isBeginSerialized()) {
                        // Ignore if there's more than one note sharing same time
                        e->ignore();
                    } else {
                        // Arrange lyrics
                        m_cachedLyrics.clear();

                        QStringList lyrics;
                        const auto &selection = m_selection->begins();
                        for (const auto &pair : selection) {
                            const auto &set = pair.second;
                            auto note = *set.begin();
                            lyrics.append(note->lyric.isEmpty() ? "\\" : note->lyric);
                            m_cachedLyrics.append(qMakePair(note, note->lyric));
                        }
                        e->text = lyrics.join(LYRICS_SEPARATOR);
                        m_editing = true;
                    }
                    break;
                }
                case QEventImpl::StdinRequestEvent::InputUpdate: {
                    if (m_editing) {
                        auto lyrics = Math::splitAll(e->text, QChar(LYRICS_SEPARATOR));

                        // Recover cached notes
                        for (const auto &pair : qAsConst(m_cachedLyrics)) {
                            auto note = pair.first;
                            note->lyric = pair.second;
                            note->update();
                        }

                        // Change new notes
                        m_cachedLyrics.clear();
                        QList<TNRectNote *> curNotes = tryApplyLyrics(lyrics.size());

                        for (int i = 0; i < curNotes.size(); ++i) {
                            auto note = curNotes.at(i);
                            m_cachedLyrics.append(qMakePair(note, note->lyric));

                            QString lrc = lyrics.at(i);
                            if (lrc.front() == '\\') {
                                lrc = lrc.mid(1);
                            }
                            note->lyric = lrc;
                            note->update();
                        }
                    }
                    break;
                }
                case QEventImpl::StdinRequestEvent::InputCommit: {
                    if (m_editing) {
                        QList<TOLyricsChange::LyricsData> changes;

                        // Save note lyrics change
                        for (const auto &pair : qAsConst(m_cachedLyrics)) {
                            auto note = pair.first;
                            if (note->lyric != pair.second) {
                                changes.append(TOLyricsChange::LyricsData{
                                    note->id,    // Id
                                    note->lyric, // New
                                    pair.second  // Old
                                });
                            }
                        }

                        if (!changes.isEmpty()) {
                            // New Operation
                            auto op = new TOLyricsChange();
                            op->data = std::move(changes);

                            // Dispatch
                            TOperateEvent e;
                            e.setData(op);
                            e.dispatch(a);
                        }

                        m_editing = false;
                    }
                    break;
                }
                case QEventImpl::StdinRequestEvent::InputAbort: {
                    if (m_editing) {
                        // Recover cached notes
                        for (const auto &pair : qAsConst(m_cachedLyrics)) {
                            auto note = pair.first;
                            note->lyric = pair.second;
                            note->update();
                        }
                        m_editing = false;
                    }
                    break;
                }
                default:
                    break;
                }
                break;
            }
            default:
                break;
            }
        }

        case QEventImpl::SceneActionRequest: {
            auto e = static_cast<QEventImpl::SceneActionRequestEvent *>(event);
            auto act = e->action();
            switch (act) {
            case QEventImpl::SceneActionRequestEvent::Paste: {
                auto board = QApplication::clipboard();
                auto mime = board->mimeData();
                if (mime->hasFormat(Qs::MIME_CLIPBOARD_NOTE)) {
                    // From system clipboard
                    QByteArray data = mime->data(Qs::MIME_CLIPBOARD_NOTE);
                    QJsonParseError parseError;
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
                    if (parseError.error == QJsonParseError::NoError) {
                        QJsonArray arr = jsonDoc.array();

                        QList<TNRectNote *> ptrs;
                        for (auto it = arr.begin(); it != arr.end(); ++it) {
                            auto note = TWNote::NoteAll::fromJson(it->toObject());
                            if (note.id != 0) {
                                // Add New Notes (New id and gid)
                                auto p = createNote(0, note.start, note.length, note.noteNum,
                                                    note.lyric, m_currentGroup);
                                adjustGeometry(p);
                                ptrs.append(p);
                            }
                        }

                        adjustCanvas();

                        QList<TONoteInsDel::NoteData> notes;
                        for (auto note : qAsConst(ptrs)) {
                            notes.append(TONoteInsDel::NoteData{
                                note->id,        // Id
                                note->start,     // Start
                                note->length,    // Length
                                note->tone,      // Tone
                                note->lyric,     // Lyric
                                note->group->id, // Gid
                            });
                        }

                        if (!notes.isEmpty()) {
                            // New Operation
                            auto op = new TONoteInsDel(TONoteInsDel::Create);
                            op->data = std::move(notes);

                            // Dispatch
                            TOperateEvent e;
                            e.setData(op);
                            e.dispatch(a);
                        }
                    }
                }
                break;
            }
            case QEventImpl::SceneActionRequestEvent::Cut:
            case QEventImpl::SceneActionRequestEvent::Copy:
            case QEventImpl::SceneActionRequestEvent::Remove: {
                QList<TONoteInsDel::NoteData> notes;
                QList<TNRectNote *> notesToRemove;
                QJsonArray arr;
                const auto &selection = m_selection->begins();
                for (const auto &pair : selection) {
                    const auto &set = pair.second;
                    for (auto note : set) {
                        if (act != QEventImpl::SceneActionRequestEvent::Remove) {
                            arr.append(TWNote::NoteAll{
                                note->id,        // Id
                                note->start,     // Start
                                note->length,    // Length
                                note->tone,      // Tone
                                note->lyric,     // Lyric
                                note->group->id, // Gid
                            }
                                           .toJson());
                        }
                        if (act != QEventImpl::SceneActionRequestEvent::Copy) {
                            notes.append(TONoteInsDel::NoteData{
                                note->id,        // Id
                                note->start,     // Start
                                note->length,    // Length
                                note->tone,      // Tone
                                note->lyric,     // Lyric
                                note->group->id, // Gid
                            });
                            notesToRemove.append(note);
                        }
                    }
                }
                if (act != QEventImpl::SceneActionRequestEvent::Copy) {
                    // Remove Notes
                    for (auto p : qAsConst(notesToRemove)) {
                        removeNote(p);
                    }
                    if (!notes.isEmpty()) {
                        // New Operation
                        auto op = new TONoteInsDel(TONoteInsDel::Remove);
                        op->data = std::move(notes);

                        // Dispatch
                        TOperateEvent e;
                        e.setData(op);
                        e.dispatch(a);
                    }
                }
                if (act != QEventImpl::SceneActionRequestEvent::Remove) {
                    if (!arr.isEmpty()) {
                        // Append to system clipboard
                        QMimeData *data = new QMimeData();
                        data->setData(Qs::MIME_CLIPBOARD_NOTE, QJsonDocument(arr).toJson());

                        auto board = QApplication::clipboard();
                        board->setMimeData(data);
                    }
                }
                break;
            }
            case QEventImpl::SceneActionRequestEvent::SelectAll: {
                selectAll();
                break;
            }
            case QEventImpl::SceneActionRequestEvent::Deselect: {
                deselect();
                break;
            }
            case QEventImpl::SceneActionRequestEvent::Digital: {
                auto e2 = static_cast<TDigitalEvent *>(e);
                switch (e2->dType()) {
                case TDigitalEvent::Transpose: {
                    QList<TONoteMove::MoveData> moves;
                    const auto &selection = m_selection->begins();
                    for (const auto &pair : selection) {
                        const auto &set = pair.second;
                        for (auto note : set) {
                            int dy = e2->digit;
                            dy = qMin(qMax(24, note->tone + dy), 107) - note->tone;

                            note->tone += dy;
                            adjustGeometry(note);

                            if (dy != 0) {
                                moves.append(TONoteMove::MoveData{note->id, 0, dy});
                            }
                        }
                    }
                    if (!moves.isEmpty()) {
                        // New Operation
                        auto op = new TONoteMove();
                        op->data = std::move(moves);

                        // Dispatch
                        TOperateEvent e;
                        e.setData(op);
                        e.dispatch(a);
                    }
                    break;
                }
                default:
                    break;
                }
            }
            case QEventImpl::SceneActionRequestEvent::Group: {
                if (!m_selection->isEmpty()) {
                    auto g = createGroup(0, QString(), QString(), QString());
                    QList<quint64> ids;

                    // Move to new group
                    const auto &selection = m_selection->begins();
                    for (const auto &pair : selection) {
                        const auto &set = pair.second;
                        for (auto note : set) {
                            changeNoteGroup(note, g);
                            ids.append(note->id);
                        }
                    }

                    // New Operation
                    auto op = new TOGroupChange(TOGroupChange::Move);
                    op->ids = std::move(ids);
                    op->group = TOGroupChange::GroupData{g->id, g->name, g->singer, g->rom};
                    op->oldGroup =
                        TOGroupChange::GroupData{m_currentGroup->id, m_currentGroup->name,
                                                 m_currentGroup->singer, m_currentGroup->rom};

                    // Dispatch
                    TOperateEvent e;
                    e.setData(op);
                    e.dispatch(a);

                    auto oldGroup = m_currentGroup;
                    switchGroup(g);

                    if (oldGroup != m_mainGroup && oldGroup->isEmpty()) {
                        removeGroup(oldGroup);
                    }
                }
                break;
            }
            case QEventImpl::SceneActionRequestEvent::Ungroup: {
                if (!m_selection->isEmpty() && m_currentGroup != m_mainGroup) {

                    QList<quint64> ids;

                    // Move to new group
                    const auto &selection = m_selection->begins();
                    for (const auto &pair : selection) {
                        const auto &set = pair.second;
                        for (auto note : set) {
                            changeNoteGroup(note, m_mainGroup);
                            ids.append(note->id);
                        }
                    }

                    // New Operation
                    auto op = new TOGroupChange(TOGroupChange::Move);
                    op->ids = std::move(ids);
                    op->group = TOGroupChange::GroupData{m_mainGroup->id, m_mainGroup->name,
                                                         m_mainGroup->singer, m_mainGroup->rom};
                    op->oldGroup =
                        TOGroupChange::GroupData{m_currentGroup->id, m_currentGroup->name,
                                                 m_currentGroup->singer, m_currentGroup->rom};

                    // Dispatch
                    TOperateEvent e;
                    e.setData(op);
                    e.dispatch(a);

                    auto oldGroup = m_currentGroup;
                    switchGroup(m_mainGroup);

                    if (oldGroup->isEmpty()) {
                        removeGroup(oldGroup);
                    }
                }
                break;
            }
            default:
                break;
            }

            break;
        }

        case QEventImpl::SceneStateChange: {
            auto e = static_cast<QEventImpl::SceneStateChangeEvent *>(event);
            switch (e->cType()) {
            case QEventImpl::SceneStateChangeEvent::CursorMode: {
                if (a->drawMode() == TNotesArea::DrawCurves) {
                    setNotesMovable(false);
                } else {
                    setNotesMovable(true);
                }
                break;
            }
            default:
                break;
            }
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

void TNNotesCtl::_q_beginChanged(int index, int oldIndex, TNRectNote *p) {
    Q_UNUSED(oldIndex);
    Q_UNUSED(index);
    Q_UNUSED(p);
}

void TNNotesCtl::_q_endChanged(int index, int oldIndex, TNRectNote *p) {
    Q_UNUSED(oldIndex);
    Q_UNUSED(p);
    if (index == m_timeBounds->ends().size() - 1) {
        adjustCanvas();
    }
}
