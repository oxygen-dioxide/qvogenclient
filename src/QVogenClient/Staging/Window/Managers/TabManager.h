#ifndef TABMANAGER_H
#define TABMANAGER_H

#include "../CentralManager.h"
#include "CentralTab.h"

#include "Types/Actions.h"
#include "CommonTune/CommonScore.h"

class MainWindow;
class TabManagerPrivate;

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
    VogenTab *addUntitled(const CommonScore &proj);

    CentralTab *addUnique(CentralTab::Type type);

    CentralTab *currentTab() const;

    bool closeTab(int index);
    bool closeTab(CentralTab *tab);

    bool closeOthers(int index);
    bool closeSaved();
    bool closeAll();

    bool saveAll();

    void triggerCurrent(ActionImpl::Action a);
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
