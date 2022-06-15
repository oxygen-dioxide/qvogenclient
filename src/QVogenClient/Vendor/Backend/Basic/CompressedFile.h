#ifndef COMPRESSEDFILE_H
#define COMPRESSEDFILE_H

#include "SpecialFile.h"

class CompressedFile : public SpecialFile {
public:
    CompressedFile();
    ~CompressedFile();

public:
    QString tempDir() const;
    void setTempDir(const QString &tempDir);

protected:
    virtual bool loadCore(bool *valid);
    virtual bool saveCore();

    virtual void resetCore();

    QStringList m_fileList;
    QString m_tempDir;
};

#endif // COMPRESSEDFILE_H
