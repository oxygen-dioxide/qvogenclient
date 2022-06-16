#ifndef QVOGENFILE_H
#define QVOGENFILE_H

#include <QString>

#include "CompressedFile.h"

/*
 * Usage: util to parse or write Vogen package file(.ust).
 */

class QVogenFile : public CompressedFile {
public:
    QVogenFile();
    QVogenFile(const QString &filename);
    ~QVogenFile();

public:
    QString projectName;

protected:
    bool loadCore(bool *valid) override;
    bool saveCore() override;
    void resetCore() override;
};

#endif // QVOGENFILE_H
