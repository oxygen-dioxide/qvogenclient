#include "FilesActionEngine.h"
#include "FilesActionEngine_p.h"

#include <QDebug>
#include <QMessageBox>

FilesActionEngine::FilesActionEngine(QObject *parent)
    : FilesActionEngine(*new FilesActionEnginePrivate(), parent) {
}

FilesActionEngine::~FilesActionEngine() {
}

FilesActionEngine::FilesActionEngine(FilesActionEnginePrivate &d, QObject *parent)
    : ActionManagerEngine(d, parent) {
    d.init();
}

void FilesActionEngine::q_actionTriggered(QAction *action) {
    Q_D(FilesActionEngine);
    auto it = d->map.find(action);
    if (it == d->map.end()) {
        return;
    }
    d->tabMgr->triggerCurrent(it.value());
}
