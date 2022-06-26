#include "TabManager_p.h"

#include "DataManager.h"
#include "WindowManager.h"

#include "ActionManager.h"
#include "EventManager.h"
#include "MainWindow.h"

#include "FolderTab.h"
#include "VogenTab/VogenTab.h"
#include "WelcomeTab/WelcomeTab.h"

#include <QMessageBox>

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

void TabManagerPrivate::reloadActionStates(ActionImpl::StateTypes st) {
    auto tab = currentTab();

    states &= ~ActionImpl::TypeMask;
    if (tab) {
        states |= ActionImpl::TabFlag;
    }

    if (st & ActionImpl::FileState) {
        states &= ~ActionImpl::FileMask;
        if (tab) {
            if (tab->type() & CentralTab::Document) {
                auto docTab = qobject_cast<DocumentTab *>(tab);
                states |= ActionImpl::DocumentFlag;
                states |= docTab->isEdited() ? ActionImpl::EditedFlag : ActionImpl::NoFlag;
                states |= docTab->isUntitled() ? ActionImpl::UntitledFlag : ActionImpl::NoFlag;
                states |= docTab->isDeleted() ? ActionImpl::DeletedFlag : ActionImpl::NoFlag;
            }
        }
    }
    if (st & ActionImpl::EditState) {
        states &= ~ActionImpl::EditMask;
        if (tab) {
            if (tab->type() & CentralTab::Document) {
                auto docTab = qobject_cast<DocumentTab *>(tab);
                states |= ActionImpl::DocumentFlag;
                states |= docTab->canUndo() ? ActionImpl::UndoFlag : ActionImpl::NoFlag;
                states |= docTab->canRedo() ? ActionImpl::RedoFlag : ActionImpl::NoFlag;
            }
        }
    }
    if (st & ActionImpl::SelectState) {
        states &= ~ActionImpl::SelectMask;
        if (tab) {
            if (tab->type() & CentralTab::Document) {
                auto docTab = qobject_cast<DocumentTab *>(tab);
                states |= ActionImpl::DocumentFlag;
                states |= docTab->selectionFlags() ? ActionImpl::SelectionFlag : ActionImpl::NoFlag;
            }
        }
    }

    w->actionMgr()->reloadStates(st);
}

bool TabManagerPrivate::tryCloseTab(int index) {
    auto tab = tabAt(index);
    auto tabs = w->tabWidget();

    bool accept = true;
    QString title;

    if (tab->type() & CentralTab::Document) {
        auto tab1 = qobject_cast<DocumentTab *>(tab);
        title = QObject::tr("You have not saved the file, do you want to save it?");
        accept = !(tab1->isDeleted() || tab1->isEdited());
    } else if (tab->type() & CentralTab::Folder) {
        title = QObject::tr("You have not saved the directory, do you want to save it?");
        accept = !tab->isEdited();
    } else {
        title = QObject::tr("You have not saved the configuration, do you want to save it?");
        accept = !tab->isEdited();
    }

    if (!accept) {
        tabs->setCurrentIndex(index);

        QMessageBox msgbox(w);
        msgbox.setText(title);
        msgbox.setWindowTitle(qData->mainTitle());
        msgbox.setIcon(QMessageBox::Question);
        msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgbox.setDefaultButton(QMessageBox::Yes);

        msgbox.button(QMessageBox::Yes)->setText(QObject::tr("Save(&S)"));
        msgbox.button(QMessageBox::No)->setText(QObject::tr("Don't save(&N)"));
        msgbox.button(QMessageBox::Cancel)->setText(QObject::tr("Cancel"));

        QMessageBox::StandardButton result =
            static_cast<QMessageBox::StandardButton>(msgbox.exec());

        switch (result) {
        case QMessageBox::Yes:
            accept = w->eventMgr()->saveFile(tab);
            break;
        case QMessageBox::No:
            accept = true;
            break;
        default:
            accept = false;
            break;
        }
    }

    if (!accept) {
        return false;
    }

    w->tabWidget()->removeTab(index);
    tab->deleteLater();

    return true;
}

VogenTab *TabManagerPrivate::createProjectTab(const QString &filename) {
    auto tab = new VogenTab();
    tab->setFilename(filename);
    w->tabWidget()->addTab(tab, QString());
    tab->initAfterLayout();
    return tab;
}

CentralTab *TabManagerPrivate::createUniqueTab(CentralTab::Type type) {
    CentralTab *tab = nullptr;
    switch (type) {
    case CentralTab::Welcome:
        tab = new WelcomeTab();
    default:
        break;
    }
    if (tab) {
        w->tabWidget()->addTab(tab, QString());
        tab->initAfterLayout();
    }
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
