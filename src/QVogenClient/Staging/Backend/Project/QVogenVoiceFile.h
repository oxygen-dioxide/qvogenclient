#ifndef QVOGENVOICEFILE_H
#define QVOGENVOICEFILE_H

#include <QList>
#include <QString>

#include "CompressedFile.h"

class QVogenVoiceFile : public CompressedFile {
public:
    QVogenVoiceFile();
    QVogenVoiceFile(const QString &filename);
    ~QVogenVoiceFile();

    QString name;
    QString id;
    QString version;
    QString builtBy;
    QString voiceBy;

    QString dir;

    bool install();

    bool loadAndInstall();

protected:
    bool loadCore(bool *valid = nullptr) override;
    bool saveCore() override;

    void resetCore() override;
};

#endif // QVOGENVOICEFILE_H
