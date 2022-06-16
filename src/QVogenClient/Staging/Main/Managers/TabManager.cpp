#include "TabManager.h"
#include "TabManager_p.h"

#include "DocumentTab.h"
#include "MainWindow.h"

#include "DataManager.h"
#include "SystemHelper.h"

#include "EventManager.h"

#include "CMenu.h"

#include <QApplication>
#include <QEvent>

TabManager::TabManager(MainWindow *parent) : TabManager(*new TabManagerPrivate(), parent) {
}

TabManager::~TabManager() {
}

bool TabManager::load() {
    Q_D(TabManager);

    auto tabs = d->w->tabWidget();

    auto tab1 = new DocumentTab();
    tab1->setFilename("1.vog");

    auto tab2 = new DocumentTab();
    tab2->setFilename("888.vog");

    auto tab3 = new DocumentTab();
    tab3->setFilename("doaz.vog");

    tabs->addTab(tab1, QString());
    tabs->addTab(tab2, QString());
    tabs->addTab(tab3, QString());

    return true;
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
}

void TabManager::_q_tabTitleChanged(const QString &title) {
    Q_D(TabManager);
    d->w->setWindowTitle(QString("%1 - %2").arg(title, qData->windowTitle()));
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

        QAction *action = menu.exec(QCursor::pos());
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
