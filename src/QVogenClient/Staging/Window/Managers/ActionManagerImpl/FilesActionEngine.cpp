#include "FilesActionEngine.h"
#include "FilesActionEngine_p.h"

#include <QDebug>
#include <QMessageBox>

FilesActionEngine::FilesActionEngine(QObject *parent)
    : FilesActionEngine(*new FilesActionEnginePrivate(), parent) {
}

FilesActionEngine::~FilesActionEngine() {
}

void FilesActionEngine::updateStates(ActionImpl::StateTypes st) {
    Q_D(FilesActionEngine);

    auto states = d->tabMgr->actionStates();

    if (st & ActionImpl::FileState) {
        d->file_saveFile->setEnabled(states & ActionImpl::EditedFlag);
        d->file_saveAs->setEnabled(states & ActionImpl::DocumentFlag);
        d->file_appendFile->setEnabled(states & ActionImpl::DocumentFlag);
        d->file_fileSettings->setEnabled(states & ActionImpl::DocumentFlag);
        d->exportMenu->setEnabled(states & ActionImpl::DocumentFlag);
        d->file_closeFile->setEnabled(states & ActionImpl::TabFlag);
    }

    if (st & ActionImpl::EditState) {
        d->edit_undo->setEnabled(states & ActionImpl::UndoFlag);
        d->edit_redo->setEnabled(states & ActionImpl::RedoFlag);
    }

    if (st & ActionImpl::SelectState) {
        d->edit_cut->setEnabled(states & ActionImpl::SelectionFlag);
        d->edit_copy->setEnabled(states & ActionImpl::SelectionFlag);
        d->edit_paste->setEnabled(states & ActionImpl::DocumentFlag);
        d->pasteMenu->setEnabled(states & ActionImpl::DocumentFlag);
        d->edit_remove->setEnabled(states & ActionImpl::SelectionFlag);
        d->removeMenu->setEnabled(states & ActionImpl::SelectionFlag);
        d->edit_selectAll->setEnabled(states & ActionImpl::DocumentFlag);
        d->edit_deselect->setEnabled(states & ActionImpl::SelectionFlag);
    }
}

FilesActionEngine::FilesActionEngine(FilesActionEnginePrivate &d, QObject *parent)
    : ActionManagerEngine(d, parent) {
    d.init();
}

void FilesActionEngine::_q_recentCommited() {
    Q_D(FilesActionEngine);
    d->reloadRecentActions();
}

void FilesActionEngine::_q_actionTriggered(QAction *action) {
    Q_D(FilesActionEngine);
    auto menu = qobject_cast<QMenu *>(sender());
    if (menu == d->recentMenu) {
        d->handleRecentAction(action);
    } else if (menu == d->buildInMenu) {

    } else if (menu == d->pluginsMenu) {
    } else {
        auto it = d->map.find(action);
        if (it == d->map.end()) {
            return;
        }
        d->tabMgr->triggerCurrent(it.value());
    }
}
