#ifndef COMPRESSEDFILE_H
#define COMPRESSEDFILE_H

#include "SpecialFile.h"

class CompressedFile : public SpecialFile {
public:
    CompressedFile();
    ~CompressedFile();

    enum FileStat {
        NoError,
        FileNotExist,
        FileNotReadable,
        FileNotWritable,
        ExtractFailed,
        MissingComponent,
        InvalidComponent,
    };

public:
    QString tempDir() const;
    void setTempDir(const QString &tempDir);

protected:
    virtual bool loadCore(bool *valid = nullptr);
    virtual bool saveCore();

    virtual void resetCore();

    QStringList m_fileList;
    QString m_tempDir;
};

#endif // COMPRESSEDFILE_H
