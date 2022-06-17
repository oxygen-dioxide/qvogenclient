#include "TabManager_p.h"

#include "DataManager.h"
#include "WindowManager.h"

#include "MainWindow.h"

#include "FolderTab.h"
#include "VogenTab.h"

TabManagerPrivate::TabManagerPrivate() {
}

TabManagerPrivate::~TabManagerPrivate() {
}

void TabManagerPrivate::init() {
    Q_Q(TabManager);

    // Proxy All Drag Drop Events
    auto tabs = w->tabWidget();
    tabs->setAcceptDrops(false);

    // Connect Signals And Slots
    q->connect(tabs, &CentralTabWidget::currentTabTextChanged, q, &TabManager::_q_tabTitleChanged);
    q->connect(tabs, &CentralTabWidget::tabCloseRequested, q, &TabManager::_q_tabCloseRequested);
    q->connect(tabs, &CentralTabWidget::currentChanged, q, &TabManager::_q_tabIndexChanged);
    q->connect(tabs, &CentralTabWidget::tabBarClicked, q, &TabManager::_q_tabBarClicked);
}

CentralTab *TabManagerPrivate::currentTab() const {
    return qobject_cast<CentralTab *>(w->tabWidget()->currentWidget());
}

CentralTab *TabManagerPrivate::tabAt(int index) const {
    return qobject_cast<CentralTab *>(w->tabWidget()->widget(index));
}

int TabManagerPrivate::tabCount() const {
    return w->tabWidget()->count();
}

void TabManagerPrivate::reloadWindowTitle(const QString &title) {
    if (title.isEmpty()) {
        w->setWindowTitle(qData->windowTitle());
    } else {
        w->setWindowTitle(QString("%1 - %2").arg(title, qData->windowTitle()));
    }
}

bool TabManagerPrivate::tryCloseTab(int index) {
    auto tab = tabAt(index);
    w->tabWidget()->removeTab(index);
    tab->deleteLater();
    return true;
}

VogenTab *TabManagerPrivate::createProjectTab(const QString &filename) {
    auto tab = new VogenTab();
    tab->setFilename(filename);
    w->tabWidget()->addTab(tab, QString());
    return tab;
}

void TabManagerPrivate::findExistingTab(CentralTab::Type type, QPair<MainWindow *, int> *res,
                                        const QString &filename) {
    res->first = nullptr;
    res->second = -1;

    auto windows = qWindows->windows();

    for (auto it = windows.begin(); it != windows.end(); ++it) {
        MainWindow *w = *it;
        auto tabs = w->tabWidget();
        if (type & CentralTab::Document) {
            if (filename.isEmpty()) {
                return;
            }
            for (int i = 0; i < tabs->count(); ++i) {
                auto tab = qobject_cast<DocumentTab *>(tabs->widget(i));
                if (!tab || tab->isDeleted() || tab->isUntitled()) {
                    continue;
                }
                if (tab->filename().compare(filename, Qt::CaseInsensitive) == 0) {
                    res->second = i;
                    break;
                }
            }
        } else if (type & CentralTab::Folder) {
            if (filename.isEmpty()) {
                return;
            }
            for (int i = 0; i < tabs->count(); ++i) {
                FolderTab *tab = qobject_cast<FolderTab *>(tabs->widget(i));
                if (tab) {
                    if (tab->filename() == filename) {
                        res->second = i;
                        break;
                    }
                }
            }
        } else {
            for (int i = 0; i < tabs->count(); ++i) {
                auto tab = qobject_cast<CentralTab *>(tabs->widget(i));
                if (tab->type() == type) {
                    res->second = i;
                    break;
                }
            }
        }
        if (res->second >= 0) {
            res->first = w;
            break;
        }
    }
}
