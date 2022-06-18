#ifndef FILESACTIONENGINE_H
#define FILESACTIONENGINE_H

#include "ActionManagerEngine.h"
#include "Types/Actions.h"

class FilesActionEnginePrivate;

class FilesActionEngine : public ActionManagerEngine {
    Q_OBJECT
    Q_DECLARE_PRIVATE(FilesActionEngine)
public:
    explicit FilesActionEngine(QObject *parent = nullptr);
    ~FilesActionEngine();

public:
    void updateStates(ActionImpl::StateTypes st) override;

protected:
    FilesActionEngine(FilesActionEnginePrivate &d, QObject *parent = nullptr);

private:
    void _q_recentCommited();
    void _q_actionTriggered(QAction *action);
};

#endif // FILESACTIONENGINE_H
