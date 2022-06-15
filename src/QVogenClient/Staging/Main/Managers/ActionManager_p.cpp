#include "ActionManager_p.h"

#include "MainWindow.h"

#include "ActionManagerImpl/ActionManagerEngine_p.h"
#include "ActionManagerImpl/FilesActionEngine.h"

ActionManagerPrivate::ActionManagerPrivate() {
}

ActionManagerPrivate::~ActionManagerPrivate() {
}

void ActionManagerPrivate::init() {
    Q_Q(ActionManager);

    engine = new FilesActionEngine(q);

    // MenuBar
    engine->d_func()->menuBar = w->menuBar();

    // Managers
    engine->d_func()->eventMgr = w->eventMgr();
    engine->d_func()->tabMgr = w->tabMgr();
}
