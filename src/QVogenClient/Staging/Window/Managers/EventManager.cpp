#include "EventManager.h"
#include "EventManager_p.h"

#include "SystemHelper.h"

#include "CentralTab.h"
#include "MainWindow.h"

#include "CommonScore.h"
#include "FileParser.h"
#include "QVogenFile.h"

#include "TabManager.h"

#include "Types/Events.h"

#include "DataManager.h"
#include "WindowManager.h"

static const char FLAG_OPEN[] = "%PROJ%";
static const char FLAG_OPEN_FOLDER[] = "%DIR%";
static const char FLAG_SAVE[] = "%SAVE%";
static const char FLAG_IMPORT[] = "%IMPORT%";
static const char FLAG_APPEND[] = "%APPEND%";

#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
#include <QMimeData>

EventManager::EventManager(MainWindow *parent) : EventManager(*new EventManagerPrivate(), parent) {
}

EventManager::~EventManager() {
}

bool EventManager::load() {
    Q_D(EventManager);

    // Handle cursor mode
    connect(d->w->toolBar(), &CentralToolBar::cursorModeChanged, this,
            &EventManager::_q_cursorModeChanged);

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
    if (path.isEmpty()) {
        return false;
    }
    FileParser parser(d->w);
    CommonScore proj;

    bool res = parser.parseFile(path, proj);
    if (!res) {
        return false;
    }

    d->w->tabMgr()->addUntitled(proj);

    return true;
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
    if (path.isEmpty()) {
        return false;
    }
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
        case QEvent::DragEnter: {
            auto e = static_cast<QDragEnterEvent *>(event);
            const QMimeData *mime = e->mimeData();
            if (mime->hasUrls()) {
                auto urls = mime->urls();
                QStringList filenames;
                for (auto it = urls.begin(); it != urls.end(); ++it) {
                    if (it->isLocalFile()) {
                        filenames.append(Sys::removeTailSlashes(it->toLocalFile()));
                    }
                }
                bool hasImport = false;
                if (filenames.size() == 1) {
                    QString filename = filenames.front();
                    QString suffix = QFileInfo(filename).suffix();
                    if (FileParser::isSuffixParsable(suffix)) {
                        hasImport = true;
                    }
                }
                bool ok = true;
                if (!hasImport) {
                    for (const auto &filename : filenames) {
                        QString suffix = QFileInfo(filename).suffix();
                        if (suffix.compare("vog", Qt::CaseInsensitive) != 0) {
                            ok = false;
                            break;
                        }
                    }
                }
                if (ok) {
                    e->acceptProposedAction();
                }
            }
            if (e->isAccepted()) {
                return true;
            }
            break;
        }
        case QEvent::Drop: {
            auto e = static_cast<QDropEvent *>(event);
            const QMimeData *mime = e->mimeData();
            if (mime->hasUrls()) {
                auto urls = mime->urls();
                QStringList filenames;
                for (auto it = urls.begin(); it != urls.end(); ++it) {
                    if (it->isLocalFile()) {
                        filenames.append(Sys::removeTailSlashes(it->toLocalFile()));
                    }
                }
                // if only one file is dragged in, an import operation will be attempted
                bool hasImport = false;
                if (filenames.size() == 1) {
                    QString filename = filenames.front();
                    QString suffix = QFileInfo(filename).suffix();
                    if (FileParser::isSuffixParsable(suffix)) {
                        importFile(filename);
                        hasImport = true;
                    }
                }
                if (!hasImport) {
                    for (const auto &filename : filenames) {
                        openFile(filename);
                    }
                }
                e->acceptProposedAction();
            }
            if (e->isAccepted()) {
                return true;
            }
            break;
        }
        default:
            break;
        }
    }
    return BaseManager::eventFilter(obj, event);
}

void EventManager::_q_cursorModeChanged(int mode) {
    Q_D(EventManager);
    switch (mode) {
    case CentralToolBar::Select:
        d->w->tabMgr()->triggerCurrent(ActionImpl::View_Cursor_Select);
        break;
    case CentralToolBar::Sketch:
        d->w->tabMgr()->triggerCurrent(ActionImpl::View_Cursor_Sketch);
        break;
    case CentralToolBar::Free:
        d->w->tabMgr()->triggerCurrent(ActionImpl::View_Cursor_Freehand);
        break;
    default:
        break;
    }
}
