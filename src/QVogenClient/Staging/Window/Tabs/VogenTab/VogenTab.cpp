#include "VogenTab.h"
#include "VogenTab_p.h"

#include "DataManager.h"
#include "Logs/CRecordHolder.h"

#include "Macros.h"
#include "Namespace.h"

#include "QVogenFile.h"

#include "Utils/Events/TAppendEvent.h"
#include "Utils/Events/TPianoRollEvent.h"

#include "Utils/TWrappedData.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QScrollBar>

VogenTab::VogenTab(QWidget *parent) : VogenTab(*new VogenTabPrivate(), parent) {
}

VogenTab::~VogenTab() {
}

void VogenTab::reloadStrings() {
}

CentralTab::Type VogenTab::type() const {
    return Tuning;
}
VogenTab::VogenTab(VogenTabPrivate &d, QWidget *parent) : DocumentTab(d, parent) {
    d.init();
}

bool VogenTab::load() {
    Q_D(VogenTab);
    auto &vog = d->vog;

    // Open file
    vog.setFilename(d->filename);
    bool res = vog.load();

    if (!res) {
        Q_ERROR(this, qData->errorTitle(),
                tr("Failed to open file \"%1\"!").arg(QDir::toNativeSeparators(d->filename)));

        // Remove from recent
        qRecord->commitRecent(CRecordHolder::Project, CRecordHolder::Remove, d->filename);
        return false;
    }

    // Interpret
    TWProject pd;
    pd.projectName = vog.projectName;
    pd.tempo = vog.tempo;
    pd.beat = vog.beat;
    pd.accomOffset = vog.accomOffset;

    QList<TWProject::Utterance> utterances;
    for (const auto &utter : qAsConst(vog.utterances)) {
        TWProject::Utterance u;
        u.name = utter.name;
        u.singer = utter.singer;
        u.romScheme = utter.romScheme;

        QList<TWProject::Note> notes;
        for (const auto &note : utter.notes) {
            TWProject::Note p{note.pitch, note.lyric, note.rom, note.start, note.duration};
            notes.append(p);
        }
        u.notes = std::move(notes);
        utterances.append(u);
    }
    pd.utterances = std::move(utterances);

    d->piano->notesArea()->setProjectData(pd);

    // Add to recent
    qRecord->commitRecent(CRecordHolder::Project, CRecordHolder::Advance, d->filename);
    return true;
}

bool VogenTab::save() {
    Q_D(VogenTab);

    bool res = d->saveFile(d->filename);
    if (!res) {
        return false;
    }

    // Update Save State
    setDeleted(false);
    setUntitled(false);

    d->savedHistoryIndex = d->historyIndex; // Update saved history index
    setEdited(false);

    // Add to recent
    qRecord->commitRecent(CRecordHolder::Project, CRecordHolder::Advance, d->filename);
    return true;
}

bool VogenTab::saveAs(const QString &filename) {
    Q_D(VogenTab);

    bool res = d->saveFile(filename);
    if (!res) {
        return false;
    }

    // Update Filename
    setFilename(filename);

    // Update Save State
    setDeleted(false);
    setUntitled(false);

    d->savedHistoryIndex = d->historyIndex; // Update saved history index
    setEdited(false);

    // Add to recent
    qRecord->commitRecent(CRecordHolder::Project, CRecordHolder::Advance, d->filename);
    return true;
}

void VogenTab::undo() {
    Q_D(VogenTab);
    bool success = true;
    if (!d->earliest()) {
        auto op = d->historyList.at(d->historyIndex - 1);
        switch (op->type()) {
        case TBaseOperation::NoteMove:
        case TBaseOperation::NoteStretch:
        case TBaseOperation::LyricsChange:
        case TBaseOperation::NoteAddDelete:
        case TBaseOperation::GroupChange:
        case TBaseOperation::TempoTimeSig:
            success = d->piano->notesArea()->processOperation(op, true);
            break;
        default:
            break;
        }
        if (success) {
            d->historyIndex--;
            setEdited(d->savedHistoryIndex != d->historyIndex);
        }
    }
}

void VogenTab::redo() {
    Q_D(VogenTab);
    bool success = true;
    if (!d->latest()) {
        auto op = d->historyList.at(d->historyIndex);
        switch (op->type()) {
        case TBaseOperation::NoteMove:
        case TBaseOperation::NoteStretch:
        case TBaseOperation::LyricsChange:
        case TBaseOperation::NoteAddDelete:
        case TBaseOperation::GroupChange:
        case TBaseOperation::TempoTimeSig:
            success = d->piano->notesArea()->processOperation(op, false);
            break;
        default:
            break;
        }
        if (success) {
            d->historyIndex++;
            setEdited(d->savedHistoryIndex != d->historyIndex);
        }
    }
}

void VogenTab::initAfterLayout() {
    Q_D(VogenTab);
    // Init Scene States
    d->piano->notesArea()->setDrawMode(TNotesArea::PlainSelect);
    d->piano->notesArea()->view()->horizontalScrollBar()->setValue(0);
}

void VogenTab::handleSpecificAction(ActionImpl::Action a) {
    Q_D(VogenTab);
    switch (a) {
    case ActionImpl::Edit_Cut: {
        QEventImpl::SceneActionEvent e(QEventImpl::SceneActionEvent::Cut);
        qApp->sendEvent(d->piano->notesArea(), &e);
        break;
    }
    case ActionImpl::Edit_Copy: {
        QEventImpl::SceneActionEvent e(QEventImpl::SceneActionEvent::Copy);
        qApp->sendEvent(d->piano->notesArea(), &e);
        break;
    }
    case ActionImpl::Edit_Paste: {
        QEventImpl::SceneActionEvent e(QEventImpl::SceneActionEvent::Paste);
        qApp->sendEvent(d->piano->notesArea(), &e);
        break;
    }
    case ActionImpl::Edit_Remove: {
        QEventImpl::SceneActionEvent e(QEventImpl::SceneActionEvent::Remove);
        qApp->sendEvent(d->piano->notesArea(), &e);
        break;
    }
    case ActionImpl::Edit_SelectAll: {
        QEventImpl::SceneActionEvent e(QEventImpl::SceneActionEvent::SelectAll);
        qApp->sendEvent(d->piano->notesArea(), &e);
        break;
    }
    case ActionImpl::Edit_Deselect: {
        QEventImpl::SceneActionEvent e(QEventImpl::SceneActionEvent::Deselect);
        qApp->sendEvent(d->piano->notesArea(), &e);
        break;
    }
    case ActionImpl::View_Cursor_Select: {
        d->piano->notesArea()->setDrawMode(TNotesArea::PlainSelect);
        break;
    }
    case ActionImpl::View_Cursor_Sketch: {
        d->piano->notesArea()->setDrawMode(TNotesArea::DrawNote);
        break;
    }
    case ActionImpl::View_Cursor_Freehand: {
        d->piano->notesArea()->setDrawMode(TNotesArea::DrawCurves);
        break;
    }
    case ActionImpl::Modify_InsertLyrics: {
        d->inputLyrics();
        break;
    }
    case ActionImpl::Modify_Transpose: {
        d->inputTranspose();
        break;
    }
    case ActionImpl::Modify_OctaveUp: {
        d->transpose(12);
        break;
    }
    case ActionImpl::Modify_OctaveDown: {
        d->transpose(-12);
        break;
    }
    default:
        break;
    }
}

void VogenTab::makeUntitled(const QString &name) {
    setUntitled(true);
    if (name.isEmpty()) {
        Q_D(VogenTab);
        ++d->s_untitledIndex;
        d->untitledIndex = d->s_untitledIndex;
    }
    setFilename(name);
}

void VogenTab::import(const CommonScore &proj) {
    Q_D(VogenTab);

    makeUntitled(proj.name);

    // Interpret
    TWProject pd;
    pd.projectName = proj.name;
    pd.tempo = proj.tempo;
    pd.beat = proj.beat;
    pd.accomOffset = 0;

    if (pd.tempo < 10 || pd.tempo > 512) {
        pd.tempo = 120.0;
    }

    QList<TWProject::Utterance> utterances;
    for (const auto &utter : qAsConst(proj.tracks)) {
        if (utter.notes.isEmpty()) {
            continue;
        }

        TWProject::Utterance u;
        u.name = utter.name;
        u.singer = QString();
        u.romScheme = "man";

        QList<TWProject::Note> notes;
        for (const auto &note : utter.notes) {
            TWProject::Note p{note.noteNum, QString(), note.lyric, note.start, note.length};
            notes.append(p);
        }
        u.notes = std::move(notes);
        utterances.append(u);
    }
    pd.utterances = std::move(utterances);

    // Move to main group
    if (pd.utterances.size() == 1) {
        pd.utterances.front().name = Qs::MAIN_GROUP_NAME;
    }

    d->piano->notesArea()->setProjectData(pd);
}

void VogenTab::append(const CommonScore &proj) {
    Q_D(VogenTab);

    QList<TWProject::Utterance> utterances;
    for (const auto &utter : qAsConst(proj.tracks)) {
        if (utter.notes.isEmpty()) {
            continue;
        }

        TWProject::Utterance u;
        u.name = utter.name;
        u.singer = QString();
        u.romScheme = "man";

        QList<TWProject::Note> notes;
        for (const auto &note : utter.notes) {
            TWProject::Note p{note.noteNum, QString(), note.lyric, note.start, note.length};
            notes.append(p);
        }
        u.notes = std::move(notes);
        utterances.append(u);
    }

    TAppendEvent e;
    e.utterances = std::move(utterances);
    QApplication::sendEvent(d->piano->notesArea(), &e);
}

void VogenTab::setUntitled(bool untitled) {
    DocumentTab::setUntitled(untitled);
    if (!untitled) {
        Q_D(VogenTab);
        d->untitledIndex = -1;
    }

    updateTabName();
}

void VogenTab::setDeleted(bool deleted) {
    DocumentTab::setDeleted(deleted);
}

void VogenTab::customEvent(QEvent *event) {
    Q_D(VogenTab);
    switch (event->type()) {
    case QEventImpl::EditorRequest: {
        auto e = static_cast<QEventImpl::EditorRequestEvent *>(event);
        // switch type 2
        switch (e->rType()) {
        case QEventImpl::EditorRequestEvent::PianoRoll:
            d->dispatchEvent(static_cast<TPianoRollEvent *>(e));
            break;
        default:
            break;
        }
        // end switch type 2

        break;
    }
    case QEventImpl::SceneStateQuery: {
        QApplication::sendEvent(d->piano->notesArea(), event);
        break;
    }
    case QEventImpl::SceneStateChange: {
        auto e = static_cast<QEventImpl::SceneStateChangeEvent *>(event);
        // switch type 2
        switch (e->cType()) {
        case QEventImpl::SceneStateChangeEvent::TimeSig: {
            auto pair = d->piano->notesArea()->timeSig();
            d->piano->sectionsArea()->setBeat(QPoint(pair.first, pair.second));
            break;
        }
        case QEventImpl::SceneStateChangeEvent::Tempo: {
            auto tempo = d->piano->notesArea()->tempo();
            d->piano->sectionsArea()->setTempo(tempo);
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
