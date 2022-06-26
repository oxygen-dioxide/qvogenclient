#include "TabManager.h"
#include "TabManager_p.h"

#include "CMenu.h"
#include "MainWindow.h"
#include "VogenTab/VogenTab.h"

#include "DataManager.h"
#include "Logs/CRecordHolder.h"
#include "SystemHelper.h"
#include "WindowManager.h"

#include "EventManager.h"

#include "Types/Events.h"
#include "VogenTab/Utils/Events/SceneStateQuery/TSSQCursorModeEvent.h"

#include "Dialogs/VoiceManagerDialog.h"

#include <QApplication>
#include <QEvent>
#include <QMessageBox>

using namespace ActionImpl;

TabManager::TabManager(MainWindow *parent) : TabManager(*new TabManagerPrivate(), parent) {
}

TabManager::~TabManager() {
}

bool TabManager::load() {
    Q_D(TabManager);
    d->reloadWindowTitle(QString());
    return true;
}

VogenTab *TabManager::addProject(const QString &filename) {
    Q_D(TabManager);

    QPair<MainWindow *, int> res;
    d->findExistingTab(CentralTab::Tuning, &res, filename);

    // Find Existing
    int j = res.second;
    if (j >= 0) {
        auto w = res.first;
        auto tabs = w->tabWidget();
        tabs->setCurrentIndex(j);
        // w->showForward();
        return qobject_cast<VogenTab *>(tabs->currentWidget());
    }

    // Add new
    auto tabs = d->w->tabWidget();

    auto tab = d->createProjectTab(filename);
    tabs->setCurrentWidget(tab);

    if (!tab->load()) {
        closeTab(tab);
        return nullptr;
    }

    return tab;
}

VogenTab *TabManager::addUntitled(const CommonScore &proj) {
    Q_D(TabManager);

    // Add new
    auto tabs = d->w->tabWidget();

    auto tab = d->createProjectTab(QString());
    tabs->setCurrentWidget(tab);

    // Import
    tab->import(proj);

    return tab;
}

CentralTab *TabManager::addUnique(CentralTab::Type type) {
    Q_D(TabManager);

    QPair<MainWindow *, int> res;
    d->findExistingTab(type, &res);

    // Find Existing
    int j = res.second;
    if (j >= 0) {
        auto w = res.first;
        auto tabs = w->tabWidget();
        tabs->setCurrentIndex(j);
        // w->showForward();
        return qobject_cast<CentralTab *>(tabs->currentWidget());
    }

    // Add new
    auto tabs = d->w->tabWidget();

    auto tab = d->createUniqueTab(type);
    if (!tab) {
        return nullptr;
    }

    tabs->setCurrentWidget(tab);
    return tab;
}

CentralTab *TabManager::currentTab() const {
    Q_D(const TabManager);
    return qobject_cast<CentralTab *>(d->w->tabWidget()->currentWidget());
}

bool TabManager::closeTab(int index) {
    Q_D(TabManager);
    return d->tryCloseTab(index);
}

bool TabManager::closeTab(CentralTab *tab) {
    Q_D(TabManager);
    int index = d->w->tabWidget()->indexOf(tab);
    if (index < 0) {
        return false;
    }
    return closeTab(index);
}

bool TabManager::closeOthers(int index) {
    Q_D(TabManager);
    bool res = true;
    for (int i = d->tabCount() - 1; i >= 0; --i) {
        if (i == index) {
            continue;
        }
        if (!d->tryCloseTab(i)) {
            res = false;
            break;
        }
    }
    return res;
}

bool TabManager::closeSaved() {
    Q_D(TabManager);
    bool res = true;
    for (int i = d->tabCount() - 1; i >= 0; --i) {
        if (d->tabAt(i)->isEdited()) {
            continue;
        }
        if (!d->tryCloseTab(i)) {
            res = false;
            break;
        }
    }
    return res;
}

bool TabManager::closeAll() {
    Q_D(TabManager);
    bool res = true;
    for (int i = d->tabCount() - 1; i >= 0; --i) {
        if (!d->tryCloseTab(i)) {
            res = false;
            break;
        }
    }
    return res;
}

void TabManager::triggerCurrent(ActionImpl::Action a) {
    Q_D(TabManager);
    auto tab = currentTab();
    switch (a) {
    case File_New: {
        break;
    }
    case File_NewWindow: {
        qWindows->newWindow();
        break;
    }
    case File_Open: {
        d->w->eventMgr()->openFile();
        break;
    }
    case File_Import: {
        d->w->eventMgr()->importFile();
        break;
    }
    case File_Append: {
        d->w->eventMgr()->appendFile();
        break;
    }
    case File_Save: {
        d->w->eventMgr()->saveFile(tab);
        break;
    }
    case File_SaveAs: {
        d->w->eventMgr()->saveAsFile(tab);
        break;
    }
    case Edit_Undo: {
        if (tab->type() & CentralTab::Document) {
            tab->undo();
        }
        break;
    }
    case Edit_Redo: {
        if (tab->type() & CentralTab::Document) {
            tab->redo();
        }
        break;
    }
    case View_Quantization: {
        d->w->showCommands(QCommandPalette::Quantization);
        break;
    }
    case Help_Welcome: {
        addUnique(CentralTab::Welcome);
        break;
    }
    case Help_ShowActions: {
        break;
    }
    case Help_VoiceManager: {
        auto dlg = new VoiceManagerDialog(d->w);
        dlg->exec();
        dlg->deleteLater();
        break;
    }
    case Help_AboutQt: {
        QMessageBox::aboutQt(d->w, tr("About %1").arg("Qt"));
        break;
    }
    default: {
        if (tab) {
            tab->handleSpecificAction(a);
        }
        break;
    }
    }
}

States TabManager::actionStates() const {
    Q_D(const TabManager);
    return d->states;
}

TabManager::TabManager(TabManagerPrivate &d, MainWindow *parent) : CentralManager(d, parent) {
    d.init();
}

bool TabManager::eventFilter(QObject *obj, QEvent *event) {
    Q_D(TabManager);
    auto window = d->w;
    if (obj == window) {
        switch (event->type()) {
        // Proxy Drag And Drop Events To Tabs
        case QEvent::DragEnter:
        case QEvent::Drop: {
            auto tabs = window->tabWidget();
            auto bar = QScrollableTabBar::currentDraggedTabBar();
            MainWindow *fromWindow;
            if (bar && (fromWindow = qobject_cast<MainWindow *>(bar->window())) && 1) {
                tabs->event(event);
                if (event->isAccepted()) {
                    return true;
                }
            }
            break;
        }
        case QEvent::Close: {
            event->setAccepted(closeAll());
            break;
        }

        case QEventImpl::MenuUpdateRequest: {
            auto e = static_cast<QEventImpl::MenuUpdateRequestEvent *>(event);
            d->reloadActionStates(ActionImpl::StateTypes(e->menuIndex()));
        }

        case QEventImpl::SceneStateChange: {
            auto e = static_cast<QEventImpl::SceneStateChangeEvent *>(event);
            switch (e->cType()) {
            case QEventImpl::SceneStateChangeEvent::CursorMode: {
                auto tab = d->w->tabMgr()->currentTab();
                if (tab) {
                    TSSQCursorModeEvent e2;
                    // Query
                    QApplication::sendEvent(tab, &e2);
                    // Set
                    d->w->toolBar()->setCursorMode(
                        static_cast<CentralToolBar::CursorModes>(e2.mode));
                }
                break;
            }
            default: {
                // Proxy event
                auto tab = d->w->tabMgr()->currentTab();
                if (tab) {
                    QApplication::sendEvent(tab, e);
                }
                break;
            }
            }
        }
        default:
            break;
        }
    }
    return BaseManager::eventFilter(obj, event);
}

void TabManager::_q_tabCloseRequested(int index) {
    Q_D(TabManager);
    d->tryCloseTab(index);
}

void TabManager::_q_tabIndexChanged(int index, int orgIndex) {
    Q_UNUSED(index)
    Q_UNUSED(orgIndex)

    Q_D(TabManager);

    // Update All Menus
    QEventImpl::MenuUpdateRequestEvent e(ActionImpl::StateMask);
    QApplication::sendEvent(d->w, &e);

    // Get Scene State
    auto tab = d->w->tabMgr()->currentTab();
    if (tab && tab->type() == CentralTab::Tuning) {
        TSSQCursorModeEvent e2;
        // Query
        QApplication::sendEvent(tab, &e2);
        // Set
        d->w->toolBar()->setCursorMode(static_cast<CentralToolBar::CursorModes>(e2.mode));
    }
}

void TabManager::_q_tabTitleChanged(const QString &title) {
    Q_D(TabManager);
    d->reloadWindowTitle(title);
}

void TabManager::_q_tabBarClicked(Qt::MouseButton button, int index) {
    Q_D(TabManager);
    if (button == Qt::RightButton) {
        auto tab = d->tabAt(index);
        if (!tab) {
            return;
        }

        CMenu menu(d->w);

        QAction closeAction(tr("Close"), &menu);
        QAction closeOthersAction(tr("Close Others"), &menu);
        QAction closeSavedAction(tr("Close Saved"), &menu);
        QAction closeAllAction(tr("Close All"), &menu);
        QAction revealAction(&menu);

        menu.addAction(&closeAction);
        menu.addAction(&closeOthersAction);
        menu.addAction(&closeSavedAction);
        menu.addAction(&closeAllAction);

        if ((tab->type() & CentralTab::Document) && Sys::isFileExist(tab->filename())) {
            revealAction.setText(tr("Show in %1(&S)").arg(qData->fileManagerName()));
            menu.addSeparator();
            menu.addAction(&revealAction);
        } else if ((tab->type() & CentralTab::Folder) && Sys::isDirExist(tab->filename())) {
            revealAction.setText(tr("Open in %1(&S)").arg(qData->fileManagerName()));
            menu.addSeparator();
            menu.addAction(&revealAction);
        }

        auto action = menu.exec(QCursor::pos());
        d->w->tabWidget()->invalidateHover();

        if (action == &closeAction) {
            d->tryCloseTab(index);
        } else if (action == &closeOthersAction) {
            closeOthers(index);
        } else if (action == &closeSavedAction) {
            closeSaved();
        } else if (action == &closeAllAction) {
            closeAll();
        } else if (action == &revealAction) {
            Sys::reveal(d->tabAt(index)->filename());
        }
    }
}
