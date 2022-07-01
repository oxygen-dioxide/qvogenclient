#ifndef RHUTILS_H
#define RHUTILS_H

#include "Global.h"

#include <QJsonObject>

RENDER_HOST_BEGIN_NAMESPACE

QString makeRandomString(int length);

class VoiceLibMetadata {
public:
    QString name;
    QString id;
    QString version;
    QString builtBy;
    QString voiceBy;

    static VoiceLibMetadata fromObject(const QJsonObject &obj);
    QJsonObject toObject() const;

    friend QDebug operator<<(QDebug debug, const VoiceLibMetadata &meta) {
        debug.noquote() << QString("VoiceLibMetadata(%1,%2,%3,%4,%5)")
                               .arg(meta.name, meta.id, meta.version, meta.builtBy, meta.voiceBy);
        return debug;
    }
};

class TPhoneme {
public:
    int ph;
    int on;
    int off;

    TPhoneme();
    QJsonObject toObject() const;
};

class TNote {
public:
    int pitch;
    int on;
    int off;

    TNote();
    QJsonObject toObject() const;
};

class TChar {
public:
    QString ch;
    QString rom;
    QList<TNote> notes;
    QList<TPhoneme> ipa;

    TChar();
    QJsonObject toObject() const;
};

class FNote {
public:
    int pitch;
    QString lyric;
    QString rom;
    int start;
    int duration;

    QJsonObject toObject() const;
};

class FUtt {
public:
    QString name;
    QString singerId;
    QString romScheme;
    QList<FNote> notes;

    QJsonObject toObject() const;
};

class SynthArgs {
public:
    double bpm0;
    FUtt utterance;
    QString path;

    QJsonObject toObject() const;
};

RENDER_HOST_END_NAMESPACE

#endif // RHUTILS_H
