#include "TempDirGuard.h"

#include "DataManager.h"
#include "SystemHelper.h"

TempDirGuard::TempDirGuard() {
    m_autoDestroy = true;
}

TempDirGuard::~TempDirGuard() {
    if (m_autoDestroy) {
        destroy();
    }
}

void TempDirGuard::create() {
    m_path = qData->allocGlobalTempDirName();
    if (!Sys::mkDir(m_path)) {
        m_path.clear();
        return;
    }
}

void TempDirGuard::destroy() {
    if (m_path.isEmpty()) {
        return;
    }
    if (!Sys::isDirExist(m_path)) {
        goto out;
    }
    if (!QDir(m_path).removeRecursively()) {
        return;
    }
out:
    m_path.clear();
}

QString TempDirGuard::path() const {
    return m_path;
}

bool TempDirGuard::autoDestroy() const {
    return m_autoDestroy;
}

void TempDirGuard::setAutoDestroy(bool autoDestroy) {
    m_autoDestroy = autoDestroy;
}
