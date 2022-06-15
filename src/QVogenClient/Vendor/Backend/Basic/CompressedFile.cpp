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

    qDebug() << "Files count:" << files.size();
    qDebug() << "Extract time:" << timer.elapsed() << "ms";

    qDebug() << files;

    return true;
}

bool CompressedFile::saveCore() {
    return true;
}

void CompressedFile::resetCore() {
    m_fileList.clear();
}
