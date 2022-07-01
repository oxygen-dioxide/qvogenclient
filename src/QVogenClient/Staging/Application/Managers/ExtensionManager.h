#ifndef EXTENSIONSMANAGER_H
#define EXTENSIONSMANAGER_H

#include <QColor>
#include <QFile>

#include "BaseManager.h"
#include "Macros.h"

#include "RenderHost.h"

#define qTheme ExtensionManager::instance()

class ExtensionManagerPrivate;

class ExtensionManager : public BaseManager {
    Q_OBJECT
    Q_SINGLETON(ExtensionManager)
    Q_DECLARE_PRIVATE(ExtensionManager)
public:
    explicit ExtensionManager(QObject *parent = nullptr);
    ~ExtensionManager();

public:
    bool load() override;
    bool save() override;

public:
    void themeLoad(int index);
    int themeCount() const;
    QString themeName(int index) const;

    void reloadVoiceList();
    const QList<RH::VoiceLibMetadata> &voiceList() const;

    RH::RenderHost *server() const;

protected:
    ExtensionManager(ExtensionManagerPrivate &d, QObject *parent = nullptr);
};

#endif // EXTENSIONSMANAGER_H
