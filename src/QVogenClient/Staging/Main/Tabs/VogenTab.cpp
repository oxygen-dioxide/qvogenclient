#include "VogenTab.h"
#include "VogenTab_p.h"

#include "DataManager.h"

#include "Macros.h"
#include "QVogenFile.h"

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

    // Check file permissions
    QFileInfo info(d->filename);
    if (!info.isFile()) {
        Q_ERROR(this, qData->errorTitle(), tr("File does not exist."));
        return false;
    }
    if (!info.isWritable() || !info.isReadable()) {
        Q_ERROR(this, qData->errorTitle(), tr("No permission to read or write file!"));
        return false;
    }

    // Open file
    auto &file = d->vog;
    file.setFilename(info.absoluteFilePath());
    bool res = file.load();

    if (!res) {
        Q_ERROR(this, qData->errorTitle(), tr("Failed to open file!"));
        return false;
    }

    return true;
}

bool VogenTab::save() {
    Q_D(VogenTab);

    bool res = true;
    if (!res) {
        Q_ERROR(this, qData->errorTitle(), tr("Failed to save file!"));
        return false;
    }

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
