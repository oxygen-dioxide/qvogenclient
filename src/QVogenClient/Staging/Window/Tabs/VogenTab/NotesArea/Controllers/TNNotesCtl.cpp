#include "TNNotesCtl.h"
#include "../TNotesArea.h"
#include "../TNotesScroll.h"

#include "Types/Events.h"
#include "Types/Graphics.h"

#include "../../Utils/Events/PianoRoll/TChangeVoiceEvent.h"
#include "../../Utils/Events/SceneStateChange/TSSCSceneRectEvent.h"
#include "../../Utils/Events/TAppendEvent.h"
#include "../../Utils/Events/TDigitalEvent.h"
#include "../../Utils/Events/TOperateEvent.h"
#include "../../Utils/Events/TRubberBandEvent.h"

#include "../../Utils/Operations/TOGroupChange.h"
#include "../../Utils/Operations/TOLyricsChange.h"
#include "../../Utils/Operations/TONoteInsDel.h"
#include "../../Utils/Operations/TONoteMove.h"
#include "../../Utils/Operations/TONoteStretch.h"
#include "../../Utils/Operations/TOSingerChange.h"

#include "MathHelper.h"
#include "ViewHelper.h"

#include "Namespace.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#include <QJsonDocument>

#include <cstdio>

static const char LYRICS_SEPARATOR = ' ';

TNNotesCtl::TNNotesCtl(TNotesArea *parent) : TNController(parent) {
    m_timeBounds = new TNNoteList(this);
    m_selection = new TNNoteList(this);
    m_cachedSelection = new TNNoteList(this);

    m_maxNoteId = 0;
    m_maxGroupId = 0;

    m_movedNoteIndex = -1;

    // Init main group
    createGroup(0, Qs::MAIN_GROUP_NAME, QString(), QString());

    m_currentGroup = m_mainGroup;

    m_editing = false;
}

TNNotesCtl::~TNNotesCtl() {
}

void TNNotesCtl::install() {
    a->installEventFilter(this);

    m_waveScreen = new TNRectScreen(a);
    a->addItem(m_waveScreen);
    m_waveScreen->setZValue(TNotesArea::Waveform);
}

void TNNotesCtl::addUtterances(const QList<TWProject::Utterance> &utters) {
    addUtterancesCore(utters);
}

QList<TWProject::Utterance> TNNotesCtl::utterances() const {
    QList<TWProject::Utterance> utters;

    // Group List Temp
    QMap<QString, TNNoteGroup *> groups;
    for (const auto &g : qAsConst(m_noteGroups)) {
        groups.insert(g->name, g);
    }
    groups.insert(m_mainGroup->name, m_mainGroup);

    for (const auto &group : groups) {
        if (group->isEmpty()) {
            continue;
        }
        utters.append(getUtterance(group, false));
    }
    return utters;
}

TWProject::Utterance TNNotesCtl::currentValidUtterance() const {
    return getUtterance(m_currentGroup, true);
}

TWProject::Utterance TNNotesCtl::validUtterance(quint64 gid) const {
    auto g = findGroup(gid);
    if (!g) {
        return TWProject::Utterance{};
    }
    return getUtterance(g, true);
}

void TNNotesCtl::moveNotes(const QList<TWNote::Movement> &moves) {
    QSet<TNNoteGroup *> groups;
    for (const auto &move : qAsConst(moves)) {
        auto it = m_noteMap.find(move.id);
        if (it == m_noteMap.end()) {
            return;
        }
        auto note = it.value();
        note->start += move.hMove;
        note->tone += move.vMove;

        adjustGeometry(note);

        groups.insert(note->group);
    }

    for (auto g : groups) {
        invalidGroup(g);
    }
    updatePlayState();

    adjustCanvas();
}

void TNNotesCtl::stretchNotes(const QList<TWNote::Stretch> &stretches) {
    QSet<TNNoteGroup *> groups;
    for (const auto &stretch : qAsConst(stretches)) {
        auto it = m_noteMap.find(stretch.id);
        if (it == m_noteMap.end()) {
            return;
        }
        auto note = it.value();
        note->length += stretch.hStretch;
        adjustGeometry(note);

        groups.insert(note->group);
    }

    for (auto g : groups) {
        invalidGroup(g);
    }
    updatePlayState();

    adjustCanvas();
}

void TNNotesCtl::changeLyrics(const QList<TWNote::Lyric> &lyrics) {
    QSet<TNNoteGroup *> groups;
    for (const auto &lrc : qAsConst(lyrics)) {
        auto it = m_noteMap.find(lrc.id);
        if (it == m_noteMap.end()) {
            return;
        }
        auto note = it.value();
        note->lyric = lrc.lyric;
        note->update();

        groups.insert(note->group);
    }
    for (auto g : groups) {
        invalidGroup(g);
    }
    updatePlayState();
}

void TNNotesCtl::addNotes(const QList<TWNote::NoteAll> &notes, const TWNote::Group &group) {
    auto g = findGroup(group.id);

    // Create group if necessary
    if (!g) {
        g = createGroup(group.id, group.name, group.singer, group.rom);
    }

    for (const auto &note : qAsConst(notes)) {
        auto p = createNote(note.id, note.start, note.length, note.noteNum, note.lyric, g);
        adjustGeometry(p);
    }

    invalidGroup(g);
    updatePlayState();

    adjustCanvas();
}

void TNNotesCtl::removeNotes(const QList<quint64> &ids) {
    QSet<TNNoteGroup *> groups;
    for (quint64 id : qAsConst(ids)) {
        auto it = m_noteMap.find(id);
        if (it == m_noteMap.end()) {
            continue;
        }
        auto p = it.value();
        auto oldGroup = p->group;

        removeNote(p);

        groups.insert(oldGroup);
    }

    // Switch group if necessary
    if (groups.contains(m_currentGroup)) {
        switchGroup(m_mainGroup);
    }

    // Remove empty groups
    for (auto group : qAsConst(groups)) {
        if (group != m_mainGroup && group->isEmpty()) {
            removeGroup(group);
            updateScreen();
        } else {
            invalidGroup(group);
        }
    }
    updatePlayState();

    adjustCanvas();
}

void TNNotesCtl::changeGroup(const QList<quint64> &ids, const TWNote::Group &group) {
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
    invalidGroup(g);
    updatePlayState();

    // Remove empty groups
    for (auto group : qAsConst(groups)) {
        if (group != m_mainGroup && group->isEmpty()) {
            removeGroup(group);
            updateScreen();
        } else {
            invalidGroup(group);
        }
    }
    updatePlayState();
}

void TNNotesCtl::changeSinger(quint64 gid, const QString &singerId, const QString &rom) {
    auto g = findGroup(gid);
    if (!g) {
        return;
    }

    g->singerId = singerId;
    g->rom = rom;

    g->adjustHintGeometry();

    invalidGroup(g);
    updatePlayState();
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

QList<quint64> TNNotesCtl::groupIdList() const {
    QList<quint64> ids;
    ids.append(m_mainGroup->id);
    for (const auto &g : qAsConst(m_noteGroups)) {
        ids.append(g->id);
    }
    return ids;
}

void TNNotesCtl::setGroupCache(quint64 id, const QList<double> &pitches, const QString &waveFile) {
    auto g = findGroup(id);
    if (!g) {
        return;
    }

    QWaveInfo wave;
    if (!wave.load(waveFile)) {
        return;
    }

    qDebug() << "Synth Result Samples:" << wave.totalSample();

    g->setCache(TWAudio::Audio{pitches, wave});

    updatePlayState();
    updateScreen();
}

void TNNotesCtl::removeGroupCache(quint64 id) {
    forceStopPlay();
    auto g = findGroup(id);
    if (!g) {
        return;
    }
    g->removeCache();
    updateScreen();
    updatePlayState();
}

void TNNotesCtl::removeAllCache() {
    forceStopPlay();
    for (auto g : qAsConst(m_noteGroups)) {
        g->removeCache();
    }
    m_mainGroup->removeCache();

    updateScreen();
    updatePlayState();
}

bool TNNotesCtl::hasCache(quint64 id) const {
    auto g = findGroup(id);
    if (!g) {
        return false;
    }
    return g->cache() != nullptr;
}

QList<QPair<qint64, QWaveInfo *>> TNNotesCtl::audioData() const {
    QList<QPair<qint64, QWaveInfo *>> res;

    auto traverse = [this, &res](TNNoteGroup *g) {
        auto cache = g->cache();
        if (!cache) {
            return;
        }
        qint64 startTime = a->tickToTime(g->firstBegin()) - 500;
        res.append(qMakePair(startTime, &cache->wave));
    };

    for (auto g : qAsConst(m_noteGroups)) {
        traverse(g);
    }
    traverse(m_mainGroup);

    return res;
}

void TNNotesCtl::switchGroup(TNNoteGroup *group) {
    if (m_currentGroup == group) {
        return;
    }
    m_currentGroup = group;

    auto traverse = [&](TNNoteGroup *g) {
        if (g != m_currentGroup) {
            setGroupSelected(g, false);
            setGroupEnabled(g, false);
        }
    };

    for (auto g : qAsConst(m_noteGroups)) {
        traverse(g);
    }
    traverse(m_mainGroup);

    setGroupEnabled(m_currentGroup, true);

    updatePlayState();
    updateScreen();
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
    g->singerId = singer;
    g->rom = rom.isEmpty() ? "man" : rom;

    // Insert to groups
    if (g->id > 1) {
        m_noteGroups.insert(g->id, g);
    } else {
        m_mainGroup = g;
    }

    // Handle Repeated Name
    g->name = Math::adjustRepeatedName(m_groupNames, g->name);
    m_groupNames.insert(g->name);

    g->install();
    g->adjustHintGeometry();

    qDebug() << "Group" << g->id << "Created";

    return g;
}

void TNNotesCtl::removeGroup(TNNoteGroup *g) {
    Q_ASSERT(g != m_mainGroup);
    qDebug() << "Group" << g->id << "Removed";

    g->uninstall();

    m_noteGroups.remove(g->id);
    m_groupNames.remove(g->name);

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

void TNNotesCtl::invalidGroup(TNNoteGroup *group) {
    forceStopPlay();
    group->removeCache();

    const auto &all = group->begins();
    QSet<const QSet<TNRectNote *> *> ignores;

    int prevEnd = 0;
    const QSet<TNRectNote *> *prevSet = nullptr;
    for (const auto &pair : all) {
        int tick = pair.first;
        const auto &set = pair.second;
        if (set.size() > 1) {
            ignores.insert(&set);
        }
        if (prevEnd > tick) {
            if (prevSet) {
                ignores.insert(prevSet);
            }
            ignores.insert(&set);
        }
        for (auto note : set) {
            tick = qMax(tick, note->start + note->length);
        }
        prevEnd = qMax(prevEnd, tick);
        prevSet = &set;
    }

    for (const auto &pair : all) {
        const auto &set = pair.second;
        bool ignore = ignores.contains(&set);
        for (auto note : set) {
            note->setIgnored(ignore);
        }
    }

    updateScreen();
}

void TNNotesCtl::updatePlayState() {
    TPianoRollEvent e(TPianoRollEvent::PlayState);
    e.dispatch(a);
}

void TNNotesCtl::updateScreen() {
    QRectF rect = a->view()->viewportRect();
    m_waveScreen->setPos(rect.topLeft());
    m_waveScreen->setRect(QRectF(QPointF(0, 0), rect.size()));

    int curWidth = a->currentWidth();
    int curHeight = a->currentHeight();

    QList<QPair<double, QPixmap>> pixmaps;

    auto traverse = [&](TNNoteGroup *g) {
        auto cache = g->cache();
        if (!cache) {
            return;
        }

        int startTick = g->firstBegin() - a->timeToTick(500);
        int endTick = g->firstBegin() + a->timeToTick(cache->wave.duration() * 1000.0 - 500);

        double x1 = double(startTick) / 480 * curWidth + a->zeroLine();
        double x2 = double(endTick) / 480 * curWidth + a->zeroLine();

        if (x2 < rect.left() || x1 > rect.right()) {
            return;
        }

        double W = x2 - x1;
        double H = 3 * curHeight;

        double left = qMax(0.0, rect.left() - x1);
        double right = qMin(W, rect.right() - x1);

        double W2 = right - left;
        double H2 = H / 2;

        if (W2 < 1) {
            return;
        }

        QPixmap pixmap(W2, H);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setPen((m_currentGroup == g) ? QColor(255, 255, 255, 64)
                                             : QColor(255, 255, 255, 32));
        painter.translate(-left, 0);

        // Draw Pixmap
        {
            const auto &data = cache->wave.channel1();

            double A = pow(2.0, 8.0 * cache->wave.bytesPerSample() - 1);

            int count = data.size();
            double delta = 0.5;

            double x = left;
            double y = double(H) / 2;
            while (x < right) {
                int min = INT_MAX, max = INT_MIN;
                for (int j = x * count / W; j < (x + 1) * count / W; j++) {
                    if (j > 0 && j < count) {
                        if (data[j] < min) {
                            min = data[j];
                        }
                        if (data[j] > max) {
                            max = data[j];
                        }
                    }
                }
                double y1 = y + (max * H2 / A);
                double y2 = y + (min * H2 / A);
                painter.drawLine(QPointF(x, y1), QPointF(x, y2));
                x += delta;
            }
        }

        pixmaps.append(qMakePair(x1, pixmap));
    };

    // Calculate Bounding Rects
    for (auto g : qAsConst(m_noteGroups)) {
        traverse(g);
    }
    traverse(m_mainGroup);

    // Draw
    if (!pixmaps.isEmpty()) {
        QPixmap pixmap(rect.size().toSize());
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);

        for (const auto &pair : qAsConst(pixmaps)) {
            const auto &bmp = pair.second;
            double pos = qMax(0.0, pair.first - rect.left());
            painter.drawPixmap(
                QRect(QPoint(pos, rect.height() - bmp.height() - curHeight), bmp.size()), bmp);
        }

        m_waveScreen->setPixmap(pixmap);
    } else {
        m_waveScreen->setPixmap(QPixmap());
    }

    m_waveScreen->update();
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

void TNNotesCtl::adjustAllGroupHintPos() {
    for (auto g : qAsConst(m_noteGroups)) {
        g->adjustHintPos();
    }
    m_mainGroup->adjustHintPos();
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

QList<TNRectNote *> TNNotesCtl::tryApplyLyrics(int len, TNNoteList *s) {
    QList<TNRectNote *> res;
    TNRectNote *lastNote = nullptr;
    TNNoteGroup *g = nullptr;

    if (len == 0) {
        return res;
    }

    // Fill Selected Notes
    const auto &selection = s->begins();
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

void TNNotesCtl::addUtterancesCore(const QList<TWProject::Utterance> &utters,
                                   QList<QPair<TNNoteGroup *, QList<TNRectNote *>>> *res) {
    if (res) {
        res->clear();
    }

    TNRectNote *firstNote = nullptr;
    for (const auto &utter : utters) {
        TNNoteGroup *g;
        QList<TNRectNote *> notesAdded;

        if (utter.name == Qs::MAIN_GROUP_NAME) {
            g = m_mainGroup;
            g->singerId = utter.singer;
            g->rom = utter.romScheme;
        } else {
            if (utter.notes.isEmpty()) {
                continue;
            }
            g = createGroup(0, utter.name, utter.singer, utter.romScheme);
        }
        const auto &notes = utter.notes;
        for (const auto &note : notes) {
            auto p = createNote(0, note.start, note.length, note.noteNum, note.rom, g);
            adjustGeometry(p);

            if (!firstNote || p->start < firstNote->start) {
                firstNote = p;
            }
            notesAdded.append(p);
        }
        invalidGroup(g);

        if (res) {
            res->append(qMakePair(g, notesAdded));
        }
    }

    adjustCanvas();
    updatePlayState();

    // Move scroll
    if (firstNote) {
        a->setVisionFitToItem(firstNote, Qt::AnchorVerticalCenter, false);
        a->setVisionFitToItem(firstNote, Qt::AnchorHorizontalCenter, false);
    }
}

TWProject::Utterance TNNotesCtl::getUtterance(TNNoteGroup *group, bool ignore) const {
    TWProject::Utterance u;

    u.name = group->name;
    u.singer = group->singerId;
    u.romScheme = group->rom;

    QList<TWProject::Note> notes;
    const auto &all = group->begins();
    for (const auto &pair : qAsConst(all)) {
        const auto &set = pair.second;
        for (auto note : set) {
            if (ignore && note->ignored()) {
                continue;
            }

            TWProject::Note n;
            n.start = note->start;
            n.length = note->length;
            n.noteNum = note->tone;
            n.rom = note->lyric;
            n.lyric = note->lyric;
            notes.append(n);
        }
    }

    u.notes = std::move(notes);
    return u;
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
                default:
                    toMove = true;
                    break;
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

            // Change Singer Or Rom
            if (item && item->type() == GraphicsImpl::GroupHintItem) {
                auto hint = static_cast<TNGroupHint *>(item);
                if (e->button() == Qt::LeftButton) {
                    auto g = hint->noteGroup();
                    TChangeVoiceEvent e(TChangeVoiceEvent::Singer);
                    e.groupId = g->id;
                    e.singerId = g->singerId;
                    e.rom = g->rom;
                    e.dispatch(a);

                    QString oldSingerId = g->singerId;

                    if (oldSingerId != e.singerId) {
                        g->singerId = e.singerId;
                        g->adjustHintGeometry();

                        auto op = new TOSingerChange();
                        op->rom = op->oldRom = g->rom;
                        op->singerId = g->singerId;
                        op->oldSingerId = oldSingerId;
                        op->gid = g->id;

                        TOperateEvent e;
                        e.setData(op);
                        e.dispatch(a);

                        invalidGroup(g);
                        updatePlayState();
                    }
                } else if (e->button() == Qt::RightButton) {
                    auto g = hint->noteGroup();
                    TChangeVoiceEvent e(TChangeVoiceEvent::Rom);
                    e.groupId = g->id;
                    e.singerId = g->singerId;
                    e.rom = g->rom;
                    e.dispatch(a);

                    QString oldRom = g->rom;

                    if (oldRom != e.rom) {
                        g->rom = e.rom;
                        g->adjustHintGeometry();

                        auto op = new TOSingerChange();
                        op->rom = g->rom;
                        op->oldRom = oldRom;
                        op->singerId = op->oldSingerId = g->singerId;
                        op->gid = g->id;

                        TOperateEvent e;
                        e.setData(op);
                        e.dispatch(a);

                        invalidGroup(g);
                        updatePlayState();
                    }
                }
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
                        // Interrupt
                        forceStopPlay();
                        sendInterrupt();

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
                        // Interrupt
                        forceStopPlay();
                        sendInterrupt();

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
                        // Interrupt
                        forceStopPlay();
                        sendInterrupt();

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

                        invalidGroup(m_currentGroup);
                        updatePlayState();
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

                        invalidGroup(m_currentGroup);
                        updatePlayState();
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
                        QList<TONoteInsDel::NoteData> opNotes{TONoteInsDel::NoteData{
                            note->id,        // Id
                            note->start,     // Start
                            note->length,    // Length
                            note->tone,      // Tone
                            note->lyric,     // Lyric
                            note->group->id, // Gid
                        }};

                        TONoteInsDel::GroupData opGroup{
                            m_currentGroup->id,
                            m_currentGroup->name,
                            m_currentGroup->singerId,
                            m_currentGroup->rom,
                        };

                        // New Operation
                        auto op = new TONoteInsDel(TONoteInsDel::Create);
                        TONoteInsDel::UtteranceData utt;
                        utt.notes = std::move(opNotes);
                        utt.group = std::move(opGroup);
                        op->data.append(utt);

                        // Dispatch
                        TOperateEvent e;
                        e.setData(op);
                        e.dispatch(a);

                        invalidGroup(m_currentGroup);
                        updatePlayState();
                    }
                }
            }
            break;
        }

            // Mouse Double Click Event
        case QEvent::GraphicsSceneMouseDoubleClick: {
            // Interrupt
            sendInterrupt();

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

        case QEvent::GraphicsSceneResize:
        case QEvent::GraphicsSceneMove: {
            adjustAllGroupHintPos();
            updateScreen();
            break;
        }

        case QEventImpl::EditorRequest: {
            auto e = static_cast<QEventImpl::EditorRequestEvent *>(event);
            // switch type 2
            switch (e->rType()) {
            case QEventImpl::EditorRequestEvent::PianoRoll: {
                auto e2 = static_cast<TPianoRollEvent *>(e);
                // switch type 3
                switch (e2->pType()) {
                case TPianoRollEvent::RubberBand: {
                    auto e3 = static_cast<TRubberBandEvent *>(event);
                    const auto &all = m_currentGroup->begins();
                    for (const auto &pair : all) {
                        const auto &set = pair.second;
                        for (auto note : set) {
                            if (!note->isSelected() &&
                                View::rectHitTest(QRectF(note->pos(), note->rect().size()),
                                                  e3->rect())) {
                                selectOne(note);
                            }
                        }
                    }
                    break;
                }
                default:
                    break;
                }
                // end switch type 3

                break;
            }

            default:
                break;
            }
            // end switch type 2

            break;
        }

        case QEventImpl::StdinRequest: {
            auto e = static_cast<QEventImpl::StdinRequestEvent *>(event);
            // switch type 2
            switch (e->iType()) {
            case QEventImpl::StdinRequestEvent::Lyrics: {
                // Handle Lyrics Input
                // switch type 3
                switch (e->iProcess()) {
                case QEventImpl::StdinRequestEvent::InputStart: {
                    if (m_selection->isEmpty() || !m_selection->isBeginSerialized()) {
                        // Ignore if there's more than one note sharing same time
                        e->ignore();
                    } else {
                        // Arrange lyrics
                        m_cachedLyrics.clear();
                        m_cachedSelection->copy(m_selection);

                        QStringList lyrics;
                        const auto &selection = m_cachedSelection->begins();
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
                        QList<TNRectNote *> curNotes =
                            tryApplyLyrics(lyrics.size(), m_cachedSelection);

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

                            invalidGroup(m_currentGroup);
                            updatePlayState();
                        }

                        m_cachedLyrics.clear();
                        m_cachedSelection->clear();
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
                        m_cachedLyrics.clear();
                        m_cachedSelection->clear();
                        m_editing = false;
                    }
                    break;
                }
                default:
                    break;
                }
                // end switch type 3

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
            auto act = e->aType();
            switch (act) {
            case QEventImpl::SceneActionEvent::Paste: {
                auto board = QApplication::clipboard();
                auto mime = board->mimeData();
                if (mime->hasFormat(Qs::MIME_CLIPBOARD_NOTE)) {
                    // From system clipboard
                    QByteArray data = mime->data(Qs::MIME_CLIPBOARD_NOTE);
                    QJsonParseError parseError;
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);
                    if (parseError.error == QJsonParseError::NoError) {
                        QJsonArray arr = jsonDoc.array();

                        // Get note tick offset
                        int tickOffset = 0;
                        if (!m_selection->isEmpty()) {
                            auto p = *m_selection->ends().back().second.rbegin();
                            tickOffset = p->start + p->length;
                        } else if (!m_currentGroup->isEmpty()) {
                            auto p = *m_currentGroup->ends().back().second.rbegin();
                            tickOffset = p->start + p->length;
                        }

                        // Subtract minimum tick
                        QList<TWNote::NoteAll> notesToPaste;
                        int minStart = -1;
                        for (auto it = arr.begin(); it != arr.end(); ++it) {
                            auto note = TWNote::NoteAll::fromJson(it->toObject());
                            if (note.id != 0) {
                                notesToPaste.append(note);
                                minStart = (minStart < 0) ? note.start : qMin(note.start, minStart);
                            }
                        }
                        tickOffset -= minStart;

                        QList<TNRectNote *> ptrs;
                        for (const auto &note : qAsConst(notesToPaste)) {
                            // Add New Notes (New id and gid)
                            auto p = createNote(0, tickOffset + note.start, note.length,
                                                note.noteNum, note.lyric, m_currentGroup);
                            adjustGeometry(p);
                            ptrs.append(p);
                        }

                        if (!ptrs.isEmpty()) {
                            adjustCanvas();
                            deselect();

                            QList<TONoteInsDel::NoteData> opNotes;
                            for (auto note : qAsConst(ptrs)) {
                                opNotes.append(TONoteInsDel::NoteData{
                                    note->id,        // Id
                                    note->start,     // Start
                                    note->length,    // Length
                                    note->tone,      // Tone
                                    note->lyric,     // Lyric
                                    note->group->id, // Gid
                                });
                                selectOne(note);
                            }

                            TONoteInsDel::GroupData opGroup{
                                m_currentGroup->id,
                                m_currentGroup->name,
                                m_currentGroup->singerId,
                                m_currentGroup->rom,
                            };

                            // New Operation
                            auto op = new TONoteInsDel(TONoteInsDel::Create);
                            TONoteInsDel::UtteranceData utt;
                            utt.notes = std::move(opNotes);
                            utt.group = std::move(opGroup);
                            op->data.append(utt);

                            // Dispatch
                            TOperateEvent e;
                            e.setData(op);
                            e.dispatch(a);

                            invalidGroup(m_currentGroup);
                            updatePlayState();
                        }
                    }
                }

                break;
            }
            case QEventImpl::SceneActionEvent::Cut:
            case QEventImpl::SceneActionEvent::Copy:
            case QEventImpl::SceneActionEvent::Remove: {
                QList<TONoteInsDel::NoteData> opNotes;

                QList<TNRectNote *> notesToRemove;
                QJsonArray arr;
                const auto &selection = m_selection->begins();
                for (const auto &pair : selection) {
                    const auto &set = pair.second;
                    for (auto note : set) {
                        if (act != QEventImpl::SceneActionEvent::Remove) {
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
                        if (act != QEventImpl::SceneActionEvent::Copy) {
                            opNotes.append(TONoteInsDel::NoteData{
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
                if (act != QEventImpl::SceneActionEvent::Copy) {
                    // Cut or Remove
                    TONoteInsDel::GroupData opGroup{
                        m_currentGroup->id,
                        m_currentGroup->name,
                        m_currentGroup->singerId,
                        m_currentGroup->rom,
                    };

                    // Remove Notes
                    for (auto p : qAsConst(notesToRemove)) {
                        removeNote(p);
                    }

                    // Remove group if empty
                    if (m_currentGroup != m_mainGroup && m_currentGroup->isEmpty()) {
                        auto oldGroup = m_currentGroup;

                        switchGroup(m_mainGroup);

                        removeGroup(oldGroup);
                        updateScreen();
                    } else {
                        invalidGroup(m_currentGroup);
                    }
                    updatePlayState();

                    if (!opNotes.isEmpty()) {
                        // New Operation
                        auto op = new TONoteInsDel(TONoteInsDel::Remove);

                        TONoteInsDel::UtteranceData utt;
                        utt.notes = std::move(opNotes);
                        utt.group = std::move(opGroup);
                        op->data.append(utt);

                        // Dispatch
                        TOperateEvent e;
                        e.setData(op);
                        e.dispatch(a);
                    }
                }
                if (act != QEventImpl::SceneActionEvent::Remove) {
                    // Cut or Copy
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
            case QEventImpl::SceneActionEvent::SelectAll: {
                selectAll();
                break;
            }
            case QEventImpl::SceneActionEvent::Deselect: {
                deselect();
                break;
            }
            case QEventImpl::SceneActionEvent::Append: {
                auto e2 = static_cast<TAppendEvent *>(e);
                auto utterances = e2->utterances;

                // Get note tick offset
                int tickOffset = 0;
                if (!m_selection->isEmpty()) {
                    auto p = *m_selection->ends().back().second.rbegin();
                    tickOffset = p->start + p->length;
                } else if (!m_currentGroup->isEmpty()) {
                    auto p = *m_currentGroup->ends().back().second.rbegin();
                    tickOffset = p->start + p->length;
                }

                // Subtract minimum tick
                int minStart = -1;
                for (const auto &u : qAsConst(utterances)) {
                    const auto &notes = u.notes;
                    for (const auto &note : qAsConst(notes)) {
                        minStart = (minStart < 0) ? note.start : qMin(note.start, minStart);
                    }
                }
                tickOffset -= minStart;
                for (auto &u : utterances) {
                    auto &notes = u.notes;
                    for (auto &note : notes) {
                        note.start += tickOffset;
                    }
                }

                QList<QPair<TNNoteGroup *, QList<TNRectNote *>>> res;
                addUtterancesCore(utterances, &res);

                // Save New Utterances
                {
                    QList<TONoteInsDel::UtteranceData> data;
                    for (const auto &pair : qAsConst(res)) {
                        const auto &group = pair.first;
                        const auto &notes = pair.second;

                        QList<TONoteInsDel::NoteData> opNotes;
                        for (auto note : qAsConst(notes)) {
                            opNotes.append(TONoteInsDel::NoteData{
                                note->id,        // Id
                                note->start,     // Start
                                note->length,    // Length
                                note->tone,      // Tone
                                note->lyric,     // Lyric
                                note->group->id, // Gid
                            });
                        }

                        TONoteInsDel::GroupData opGroup{
                            group->id,
                            group->name,
                            group->singerId,
                            group->rom,
                        };

                        TONoteInsDel::UtteranceData utteranceData;
                        utteranceData.notes = std::move(opNotes);
                        utteranceData.group = std::move(opGroup);
                        data.append(utteranceData);
                    }

                    // New Operation
                    auto op = new TONoteInsDel(TONoteInsDel::Create);
                    op->data = std::move(data);

                    // Dispatch
                    TOperateEvent e;
                    e.setData(op);
                    e.dispatch(a);
                }

                break;
            }
            case QEventImpl::SceneActionEvent::Digital: {
                auto e2 = static_cast<TDigitalEvent *>(e);
                // switch type 2
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

                        invalidGroup(m_currentGroup);
                        updatePlayState();
                    }
                    break;
                }
                default:
                    break;
                }
                // end switch type 2

                break;
            }
            case QEventImpl::SceneActionEvent::Group: {
                if (!m_selection->isEmpty() && (m_selection->count() < m_currentGroup->count() ||
                                                m_currentGroup == m_mainGroup)) {
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
                    op->group = TOGroupChange::GroupData{g->id, g->name, g->singerId, g->rom};
                    op->oldGroup =
                        TOGroupChange::GroupData{m_currentGroup->id, m_currentGroup->name,
                                                 m_currentGroup->singerId, m_currentGroup->rom};

                    // Dispatch
                    TOperateEvent e;
                    e.setData(op);
                    e.dispatch(a);

                    auto oldGroup = m_currentGroup;
                    switchGroup(g);
                    invalidGroup(g);

                    if (oldGroup != m_mainGroup && oldGroup->isEmpty()) {
                        removeGroup(oldGroup);
                        updateScreen();
                    } else {
                        invalidGroup(oldGroup);
                    }
                    updatePlayState();
                }

                break;
            }
            case QEventImpl::SceneActionEvent::Ungroup: {
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
                                                         m_mainGroup->singerId, m_mainGroup->rom};
                    op->oldGroup =
                        TOGroupChange::GroupData{m_currentGroup->id, m_currentGroup->name,
                                                 m_currentGroup->singerId, m_currentGroup->rom};

                    // Dispatch
                    TOperateEvent e;
                    e.setData(op);
                    e.dispatch(a);

                    auto oldGroup = m_currentGroup;
                    switchGroup(m_mainGroup);
                    invalidGroup(m_mainGroup);

                    if (oldGroup->isEmpty() && oldGroup != m_mainGroup) {
                        removeGroup(oldGroup);
                        updateScreen();
                    } else {
                        invalidGroup(oldGroup);
                    }
                    updatePlayState();
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
            // switch type 2
            switch (e->cType()) {
            case QEventImpl::SceneStateChangeEvent::CursorMode: {
                if (a->drawMode() == TNotesArea::DrawCurves) {
                    setNotesMovable(false);
                } else {
                    setNotesMovable(true);
                }
                break;
            }
            case QEventImpl::SceneStateChangeEvent::SceneRect: {
                auto e = static_cast<TSSCSceneRectEvent *>(event);
                if (e->size != e->oldSize) {
                    adjustAllGeometry();
                }
                adjustAllGroupHintPos();
                updateScreen();
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
