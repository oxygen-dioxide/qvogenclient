#include "VogenTab.h"
#include "VogenTab_p.h"

#include "DataManager.h"

#include "Macros.h"
#include "QVogenFile.h"

#include "Types/Events.h"

#include "Utils/TWrappedData.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>

VogenTab::VogenTab(QWidget *parent) : VogenTab(*new VogenTabPrivate(), parent) {
}

VogenTab::~VogenTab() {
}

void VogenTab::reloadStrings() {
    updateTabName();
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
        return false;
    }

    // Interpret
    TWProject pd;
    pd.projectName = vog.projectName;
    pd.tempo = vog.tempo;
    pd.beat = vog.beat;
    pd.accomOffset = vog.accomOffset;

    QList<TWProject::Utterance> utterances;
    for (const auto &utter : vog.utterances) {
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

    return true;
}

bool VogenTab::save() {
    Q_D(VogenTab);

    bool res = d->vog.save();
    if (!res) {
        Q_ERROR(this, qData->errorTitle(), tr("Failed to save file!"));
        return false;
    }

    return true;
}

bool VogenTab::saveAs(const QString &filename) {
    Q_D(VogenTab);

    QVogenFile file(filename);

    file.projectName = d->vog.projectName;
    file.tempo = d->vog.tempo;
    file.beat = d->vog.beat;
    file.accomOffset = d->vog.accomOffset;
    file.utterances = d->vog.utterances;

    if (!file.save()) {
        Q_ERROR(this, qData->errorTitle(), tr("Failed to save file!"));
        return false;
    }

    d->vog = std::move(file);

    // Update
    setFilename(filename);

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

void VogenTab::handleSpecificAction(ActionImpl::Action a) {
    Q_D(VogenTab);
    switch (a) {
    case ActionImpl::Edit_Cut: {
        QEventImpl::SceneActionRequestEvent e(QEventImpl::SceneActionRequestEvent::Cut);
        qApp->sendEvent(d->piano->notesArea(), &e);
        break;
    }
    case ActionImpl::Edit_Copy: {
        QEventImpl::SceneActionRequestEvent e(QEventImpl::SceneActionRequestEvent::Copy);
        qApp->sendEvent(d->piano->notesArea(), &e);
        break;
    }
    case ActionImpl::Edit_Paste: {
        QEventImpl::SceneActionRequestEvent e(QEventImpl::SceneActionRequestEvent::Paste);
        qApp->sendEvent(d->piano->notesArea(), &e);
        break;
    }
    case ActionImpl::Edit_Remove: {
        QEventImpl::SceneActionRequestEvent e(QEventImpl::SceneActionRequestEvent::Remove);
        qApp->sendEvent(d->piano->notesArea(), &e);
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

bool VogenTab::event(QEvent *event) {
    Q_D(VogenTab);
    switch (event->type()) {
    case QEventImpl::PianoRollChange:
        d->dispatchEvent(static_cast<QEventImpl::PianoRollChangeEvent *>(event));
        break;
    default:
        break;
    }
    return DocumentTab::event(event);
}
