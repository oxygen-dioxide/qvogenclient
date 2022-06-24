#include "TNNotesCtl.h"
#include "../Elements/TNRectNote.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Actions.h"
#include "Types/Events.h"
#include "Types/Graphics.h"

#include "../../Utils/Events/TOperateEvent.h"
#include "../../Utils/Events/TSelectEvent.h"

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

    m_mainGroup = new TNNoteGroup(this);
    m_currentGroup = m_mainGroup;

    m_maxNoteId = 0;
    m_maxGroupId = 1;

    m_editing = false;
}

TNNotesCtl::~TNNotesCtl() {
}

void TNNotesCtl::install() {
    a->installEventFilter(this);
}

void TNNotesCtl::setUtterances(const QList<TWProject::Utterance> &utters) {
    for (const auto &utter : utters) {
        auto g = (utter.name == MAIN_GROUP_NAME) ? m_mainGroup : createGroup(0);

        g->name = utter.name;
        g->singer = utter.singer;
        g->rom = utter.romScheme;

        const auto &notes = utter.notes;
        for (const auto &note : notes) {
            auto p = createNote(0, note.start, note.length, note.noteNum, note.rom, g);
            Q_UNUSED(p);
        }

        // Insert to groups
        if (g != m_mainGroup) {
            m_noteGroups.insert(g->id, g);
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

QList<TWProject::Utterance> TNNotesCtl::utterances() const {
    return {};
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
        auto it = m_noteGroups.find(note.gid);
        if (it == m_noteGroups.end()) {
            continue;
        }
        auto g = it.value();
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

bool TNNotesCtl::isLyricsEditing() const {
    return m_editing;
}

bool TNNotesCtl::hasSelection() const {
    return !m_selection->isEmpty();
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

TNNoteGroup *TNNotesCtl::createGroup(quint64 id) {
    auto g = new TNNoteGroup(this);
    g->id = (id == 0) ? (++m_maxGroupId) : id;

    return g;
}

void TNNotesCtl::removeGroup(TNNoteGroup *g) {
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
        TSelectEvent e;
        e.dispatch(a);
    }
}

void TNNotesCtl::deselectOne(TNRectNote *p) {
    if (p->isSelected()) {
        p->setSelected(false);
        m_selection->remove(p);

        // Update Menu
        TSelectEvent e;
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
                            m_movingData.append(MovingData{note, note->pos(), 0, 0});
                            m_startPoint = e->scenePos();
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
                            m_startPoint = e->scenePos();
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
                            // Remove Notes
                            notes.append(TONoteInsDel::NoteData{
                                note->id,        // Id
                                note->start,     // Start
                                note->length,    // Length
                                note->tone,      // Tone
                                note->lyric,     // Lyric
                                note->group->id, // Gid
                            });
                            removeNote(note);
                        }
                    }
                }
                if (act != QEventImpl::SceneActionRequestEvent::Copy) {
                    qDebug() << act << QEventImpl::SceneActionRequestEvent::Copy;
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
