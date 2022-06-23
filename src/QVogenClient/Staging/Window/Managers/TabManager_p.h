#ifndef TABMANAGERPRIVATE_H
#define TABMANAGERPRIVATE_H

#include "../CentralManager_p.h"

#include "CentralTab.h"
#include "TabManager.h"

class MainWindow;

class TabManagerPrivate : public CentralManagerPrivate {
    Q_DECLARE_PUBLIC(TabManager)
public:
    TabManagerPrivate();
    ~TabManagerPrivate();

    void init();

    // Flags
    ActionImpl::States states;

    // Info
    CentralTab *currentTab() const;
    CentralTab *tabAt(int index) const;
    int tabCount() const;

    // State
    void reloadWindowTitle(const QString &title);
    void reloadActionStates(ActionImpl::StateTypes st);

    // Close
    bool tryCloseTab(int index);

    // Create
    VogenTab *createProjectTab(const QString &filename);
    CentralTab *createUniqueTab(CentralTab::Type type);

    static void findExistingTab(CentralTab::Type type, QPair<MainWindow *, int> *res,
                                const QString &filename = QString());
};

#endif // TABMANAGERPRIVATE_H
