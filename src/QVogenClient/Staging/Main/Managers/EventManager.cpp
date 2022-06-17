#include "EventManager.h"
#include "EventManager_p.h"

#include "MainWindow.h"

#include "CommonScore.h"

#include "FileParser.h"
#include "QVogenFile.h"

#include "TabManager.h"

#include <QEvent>

EventManager::EventManager(MainWindow *parent) : EventManager(*new EventManagerPrivate(), parent) {
}

EventManager::~EventManager() {
}

bool EventManager::load() {
    return true;
}

bool EventManager::open(const QString &filename) {
    Q_D(EventManager);
    return d->w->tabMgr()->addProject(filename);
}

bool EventManager::import(const QString &filename) {
    Q_D(EventManager);
    FileParser parser(d->w);
    CommonScore notes;
    if (parser.parseFile(filename, notes)) {
        //        addUntitledTab(notes);
        return true;
    }
    return false;
}

bool EventManager::append(const QString &filename) {
    Q_D(EventManager);
    FileParser parser(d->w);
    CommonScore notes;
    if (parser.parseFile(filename, notes)) {
        //        addUntitledTab(notes);
        return true;
    }
    return false;
}

EventManager::EventManager(EventManagerPrivate &d, MainWindow *parent) : CentralManager(d, parent) {
    d.init();
}

bool EventManager::eventFilter(QObject *obj, QEvent *event) {
    Q_D(EventManager);
    auto window = d->w;
    if (obj == window) {
        switch (event->type()) {
        // Proxy Drag And Drop Events To Tabs
        case QEvent::DragEnter:
        case QEvent::Drop: {
            break;
        }
        default:
            break;
        }
    }
    return BaseManager::eventFilter(obj, event);
}
