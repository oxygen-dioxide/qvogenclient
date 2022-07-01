#ifndef EXTENSIONMANAGERPRIVATE_H
#define EXTENSIONMANAGERPRIVATE_H

#include "BaseManager_p.h"

#include "ExtensionManager.h"

#include "RenderHost.h"

class ExtensionManagerPrivate : public BaseManagerPrivate {
    Q_DECLARE_PUBLIC(ExtensionManager)
public:
    ExtensionManagerPrivate();
    ~ExtensionManagerPrivate();

    void init();

    static void reloadAppFont();

    RH::RenderHost *host;

    QList<RH::VoiceLibMetadata> voices;
};

#endif // EXTENSIONMANAGERPRIVATE_H
