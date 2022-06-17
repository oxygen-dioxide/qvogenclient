#include "CompressedFile.h"

#include "SystemHelper.h"

#include <QDebug>
#include <QElapsedTimer>

#include "JlCompress.h"

CompressedFile::CompressedFile() {
}

CompressedFile::~CompressedFile() {
}

QString CompressedFile::tempDir() const {
    return m_tempDir;
}

void CompressedFile::setTempDir(const QString &tempDir) {
    m_tempDir = tempDir;
}

bool CompressedFile::loadCore(bool *valid) {
    Q_UNUSED(valid);

    if (!Sys::isFileExist(m_filename) || !Sys::mkDir(m_tempDir)) {
        return false;
    }

    QElapsedTimer timer;
    QStringList files;

    timer.start();
    files = JlCompress::extractDir(m_filename, m_tempDir);
    if (files.isEmpty()) {
        return false;
    }

    qDebug() << "Files count:" << files.size();
    qDebug() << "Extract time:" << timer.elapsed() << "ms";

    return true;
}

bool CompressedFile::saveCore() {
    if (!Sys::isDirExist(m_tempDir)) {
        return false;
    }

    QElapsedTimer timer;

    timer.start();
    bool res = JlCompress::compressDir(m_filename, m_tempDir);
    if (!res) {
        return false;
    }

    qDebug() << "Compress time:" << timer.elapsed() << "ms";

    return true;
}

void CompressedFile::resetCore() {
    m_fileList.clear();
}
