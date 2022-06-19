#ifndef EXTENSIONMANAGERPRIVATE_H
#define EXTENSIONMANAGERPRIVATE_H

#include "BaseManager_p.h"

#include "ExtensionManager.h"

#include "QVogenVoiceInfo.h"

class ExtensionManagerPrivate : public BaseManagerPrivate {
    Q_DECLARE_PUBLIC(ExtensionManager)
public:
    ExtensionManagerPrivate();
    ~ExtensionManagerPrivate();

    void init();

    static void reloadAppFont();

    QList<QVogenVoiceInfo> voices;
};

#endif // EXTENSIONMANAGERPRIVATE_H
