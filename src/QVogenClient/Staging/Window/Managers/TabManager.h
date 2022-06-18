#ifndef TABMANAGER_H
#define TABMANAGER_H

#include "../CentralManager.h"

#include "Types/Actions.h"

class MainWindow;
class TabManagerPrivate;

class CentralTab;
class VogenTab;

class TabManager : public CentralManager {
    Q_OBJECT
    Q_DECLARE_PRIVATE(TabManager)
public:
    explicit TabManager(MainWindow *parent = nullptr);
    ~TabManager();

public:
    bool load() override;

    // API
public:
    VogenTab *addProject(const QString &filename);

    CentralTab *currentTab() const;

    bool closeTab(int index);
    bool closeTab(CentralTab *tab);

    bool closeOthers(int index);
    bool closeSaved();
    bool closeAll();

    void triggerCurrent(ActionImpl::Action action);
    ActionImpl::States actionStates() const;

protected:
    TabManager(TabManagerPrivate &d, MainWindow *parent = nullptr);

    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void _q_tabCloseRequested(int index);
    void _q_tabIndexChanged(int index, int orgIndex);
    void _q_tabTitleChanged(const QString &title);
    void _q_tabBarClicked(Qt::MouseButton button, int index);

signals:
};

#endif // TABMANAGER_H
