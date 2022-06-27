#include "ActionManager.h"
#include "ActionManager_p.h"

#include "DataManager.h"
#include "TabManager.h"

#include "Types/Events.h"

#include "MainWindow.h"

ActionManager::ActionManager(MainWindow *parent)
    : ActionManager(*new ActionManagerPrivate(), parent) {
}

ActionManager::~ActionManager() {
}

void ActionManager::reloadStrings() {
    Q_D(ActionManager);
    d->engine->reloadStrings();
}

bool ActionManager::load() {
    Q_D(ActionManager);

    d->engine->setup();

    return true;
}

void ActionManager::reloadStates(ActionImpl::StateTypes st) {
    Q_D(ActionManager);
    d->engine->updateStates(st);
}

ActionManager::ActionManager(ActionManagerPrivate &d, MainWindow *parent)
    : CentralManager(d, parent) {
    d.init();
}

