#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include "../CentralManager.h"

class CentralTab;
class MainWindow;
class EventManagerPrivate;

class EventManager : public CentralManager {
    Q_OBJECT
    Q_DECLARE_PRIVATE(EventManager)
public:
    explicit EventManager(MainWindow *parent = nullptr);
    ~EventManager();

public:
    bool load() override;

public:
    bool newFile();
    bool openFile(const QString &filename = QString());
    bool importFile(const QString &filename = QString());
    bool appendFile(const QString &filename = QString());
    bool saveFile(CentralTab *tab);
    bool saveAsFile(CentralTab *tab);

protected:
    EventManager(EventManagerPrivate &d, MainWindow *parent = nullptr);

    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void _q_cursorModeChanged(int mode);
};

#endif // EVENTMANAGER_H
