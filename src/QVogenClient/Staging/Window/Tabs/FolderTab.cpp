#include "FolderTab.h"
#include "SystemHelper.h"

#include "FolderTab_p.h"

FolderTab::FolderTab(QWidget *parent) : FolderTab(*new FolderTabPrivate(), parent) {
}

FolderTab::~FolderTab() {
}

CentralTab::Type FolderTab::type() const {
    return Folder;
}

FolderTab::FolderTab(FolderTabPrivate &d, QWidget *parent) : CentralTab(d, parent) {
    d.init();
}

bool FolderTab::isUnreachable() const {
    Q_D(const FolderTab);
    return d->unreachable;
}

void FolderTab::setUnreachable(bool unreachable) {
    Q_D(FolderTab);
    d->unreachable = unreachable;
}

void FolderTab::setEdited(bool edited) {
    CentralTab::setEdited(edited);
    updateTabName();
}

void FolderTab::filenameSet(const QString &filename) {
    CentralTab::filenameSet(filename);
    updateTabName();
}
