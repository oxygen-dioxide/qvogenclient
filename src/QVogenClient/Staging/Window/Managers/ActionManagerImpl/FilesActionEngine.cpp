#include "FilesActionEngine.h"
#include "FilesActionEngine_p.h"

#include "Types/Events.h"

#include <QApplication>
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
        d->file_saveFile->setEnabled(states & (ActionImpl::EditedFlag | ActionImpl::UntitledFlag));
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

    if (st & ActionImpl::PlayState) {
        d->play_play->setEnabled(states & (ActionImpl::PlayFlag | ActionImpl::StopFlag));
        d->play_stop->setEnabled(states & ActionImpl::StopFlag);
        d->play_render->setEnabled(states & ActionImpl::RenderFlag);
        d->play_renderAll->setEnabled(states & ActionImpl::DocumentFlag);
        d->play_moveStart->setEnabled(states & ActionImpl::DocumentFlag);
        d->play_moveEnd->setEnabled(states & ActionImpl::DocumentFlag);
        d->play_removeCache->setEnabled((states & ActionImpl::DocumentFlag) &&
                                        !d->play_render->isEnabled());
        d->play_removeAllCache->setEnabled(states & ActionImpl::DocumentFlag);
        d->play_exportAudio->setEnabled(states & ActionImpl::DocumentFlag);
    }
}

FilesActionEngine::FilesActionEngine(FilesActionEnginePrivate &d, QObject *parent)
    : ActionManagerEngine(d, parent) {
    d.init();
}

void FilesActionEngine::_q_recentCommited(int rType) {
    Q_UNUSED(rType);

    Q_D(FilesActionEngine);
    d->reloadRecentActions();
}

void FilesActionEngine::_q_actionTriggered(QAction *action) {
    Q_D(FilesActionEngine);

    auto menu = qobject_cast<QMenu *>(sender());
    const auto &ws = action->associatedWidgets();

    if (ws.isEmpty() || ws.front() != menu) {
        return;
    }

    if (menu == d->recentMenu) {
        d->handleRecentAction(action);
    } else {
        auto it = d->map.find(action);
        if (it == d->map.end()) {
            return;
        }
        auto e = new QEventImpl::MainMenuTriggerEvent(it.value());
        QApplication::postEvent(d->window(), e); // Handle in next loop
    }
}
