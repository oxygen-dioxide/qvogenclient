#include "QVogenFile.h"

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
    bool res = CompressedFile::loadCore(valid);
    if (!res) {
        return false;
    }
    return true;
}

bool QVogenFile::saveCore() {
    return true;
}

void QVogenFile::resetCore() {
}
