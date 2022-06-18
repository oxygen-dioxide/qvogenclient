#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include "../CentralManager.h"
#include "Types/Actions.h"

#include <QAction>

class ActionManagerPrivate;
class MainWindow;

class ActionManager : public CentralManager {
    Q_OBJECT
    Q_DECLARE_PRIVATE(ActionManager)
public:
    explicit ActionManager(MainWindow *parent = nullptr);
    ~ActionManager();

    void reloadStrings();

public:
    bool load() override;

    void reloadStates(ActionImpl::StateTypes st);

protected:
    ActionManager(ActionManagerPrivate &d, MainWindow *parent = nullptr);

signals:
};

#endif // ACTIONMANAGER_H
