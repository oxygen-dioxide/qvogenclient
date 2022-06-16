#include "QVogenFile.h"
#include "TempDirGuard.h"

#include <QDebug>

#include <quazip.h>

QVogenFile::QVogenFile() {
    reset();
}

QVogenFile::QVogenFile(const QString &filename) {
    setFilename(filename);
}

QVogenFile::~QVogenFile() {
}

bool QVogenFile::loadCore(bool *valid) {
    bool alloc = false;
    bool res = false;

    TempDirGuard guard;
    if (m_tempDir.isEmpty()) {
        alloc = true;
        guard.create();
        m_tempDir = guard.path();
        if (m_tempDir.isEmpty()) {
            goto out;
        }
    }

    res = CompressedFile::loadCore(valid);
    if (!res) {
        goto out;
    }

out:
    if (alloc) {
        m_tempDir.clear();
    }
    return res;
}

bool QVogenFile::saveCore() {
    return true;
}

void QVogenFile::resetCore() {
}
