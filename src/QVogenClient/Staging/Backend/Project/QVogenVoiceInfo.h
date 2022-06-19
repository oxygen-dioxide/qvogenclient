#ifndef QVOGENVOICEINFO_H
#define QVOGENVOICEINFO_H

#include "SpecialConfig.h"

class QVogenVoiceInfo : public SpecialConfig {
public:
    QVogenVoiceInfo();
    QVogenVoiceInfo(const QString &dirname);
    ~QVogenVoiceInfo();

    QString name;
    QString id;
    QString version;
    QString builtBy;
    QString voiceBy;

protected:
    bool loadCore(const QString &filename, bool *valid) override;
    bool saveCore(const QString &filename) override;

    void resetCore() override;

    QString configName() const override;
};

#endif // QVOGENVOICEINFO_H
