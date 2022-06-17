#include "EventManager.h"
#include "EventManager_p.h"

#include "CentralTab.h"
#include "MainWindow.h"

#include "CommonScore.h"
#include "FileParser.h"
#include "QVogenFile.h"

#include "TabManager.h"

#include "DataManager.h"
#include "WindowManager.h"

static const char FLAG_OPEN[] = "%PROJ%";
// static const char FLAG_OPEN_FOLDER[] = "%DIR%";
static const char FLAG_SAVE[] = "%SAVE%";
static const char FLAG_IMPORT[] = "%IMPORT%";
static const char FLAG_APPEND[] = "%APPEND%";

#include <QEvent>

EventManager::EventManager(MainWindow *parent) : EventManager(*new EventManagerPrivate(), parent) {
}

EventManager::~EventManager() {
}

bool EventManager::load() {
    return true;
}

bool EventManager::newFile() {
    return true;
}

bool EventManager::openFile(const QString &filename) {
    Q_D(EventManager);
    QStringList paths;
    if (!filename.isEmpty()) {
        paths.append(filename);
    } else {
        paths = qData->openFiles(tr("Open"), qData->getFileFilter(DataManager::ProjectFiles),
                                 FLAG_OPEN, d->w);
    }
    bool res = false;
    for (auto it = paths.begin(); it != paths.end(); ++it) {
        res |= d->w->tabMgr()->addProject(*it) != nullptr;
    }
    return res;
}

bool EventManager::importFile(const QString &filename) {
    Q_D(EventManager);
    QString path;
    if (!filename.isEmpty()) {
        path = filename;
    } else {
        path = qData->openFile(tr("Import"), qData->getFileFilter(DataManager::ImportFile),
                               FLAG_IMPORT, d->w);
    }
    if (!path.isEmpty()) {
    }
    return false;
}

bool EventManager::appendFile(const QString &filename) {
    Q_D(EventManager);
    QString path;
    if (!filename.isEmpty()) {
        path = filename;
    } else {
        path = qData->openFile(tr("Append"), qData->getFileFilter(DataManager::AppendFile),
                               FLAG_APPEND, d->w);
    }
    if (!path.isEmpty()) {
    }
    return false;
}

bool EventManager::saveAsFile(CentralTab *tab) {
    Q_D(EventManager);
    QString path =
        qData->saveFile(tr("Save As"), tab->filename(),
                        qData->getFileFilter(DataManager::ProjectFiles), FLAG_SAVE, d->w);
    return tab->saveAs(path);
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
