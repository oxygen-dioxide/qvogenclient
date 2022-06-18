#include "VogenTab.h"
#include "VogenTab_p.h"

#include "DataManager.h"

#include "Macros.h"
#include "QVogenFile.h"

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

    // Open file
    d->vog.setFilename(d->filename);
    bool res = d->vog.load();

    if (!res) {
        Q_ERROR(this, qData->errorTitle(),
                tr("Failed to open file \"%1\"!").arg(QDir::toNativeSeparators(d->filename)));
        return false;
    }

    QList<CommonNote> notes;
    for (const auto &uttrance : d->vog.utterances) {
        for (const auto &note : uttrance.notes) {
            CommonNote p(note.start, note.duration, note.pitch, note.rom, d->vog.tempo);
            notes.append(std::move(p));
        }
    }
    d->piano->notesArea()->setNotesFromCommon(notes);

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
