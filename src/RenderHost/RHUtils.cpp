#include "RHUtils.h"
#include "Utils/QJsonFormatter.h"

#include <QRandomGenerator>

RENDER_HOST_USE_NAMESPACE;

static const char RANDOM_CHAR_SET[] =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

QString RH::makeRandomString(int length) {
    QString res;
    int len = qstrlen(RANDOM_CHAR_SET);
    int idx;
    for (int i = 0; i < length; ++i) {
        idx = QRandomGenerator::global()->bounded(len);
        QChar ch = RANDOM_CHAR_SET[idx];
        res.append(ch);
    }
    return res;
}

static const char KEY_NAME_VOICE_NAME[] = "name";
static const char KEY_NAME_VOICE_ID[] = "id";
static const char KEY_NAME_VOICE_VERSION[] = "version";
static const char KEY_NAME_VOICE_BUILT_BY[] = "builtBy";
static const char KEY_NAME_VOICE_VOICE_BY[] = "voiceBy";

static const char KEY_NAME_TPHONEME_PH[] = "ph";
static const char KEY_NAME_TPHONEME_ON[] = "on";
static const char KEY_NAME_TPHONEME_OFF[] = "off";

static const char KEY_NAME_TNOTE_PITCH[] = "pitch";
static const char KEY_NAME_TNOTE_ON[] = "on";
static const char KEY_NAME_TNOTE_OFF[] = "off";

static const char KEY_NAME_TCHAR_CH[] = "ch";
static const char KEY_NAME_TCHAR_ROM[] = "rom";
static const char KEY_NAME_TCHAR_NOTES[] = "notes";
static const char KEY_NAME_TCHAR_IPA[] = "ipa";

static const char KEY_NAME_UTTERANCE_NAME[] = "name";
static const char KEY_NAME_UTTERANCE_SINGER[] = "singerId";
static const char KEY_NAME_UTTERANCE_ROM_SCHEME[] = "romScheme";
static const char KEY_NAME_UTTERANCE_NOTES[] = "notes";

static const char KEY_NAME_NOTE_PITCH[] = "pitch";
static const char KEY_NAME_NOTE_LYRIC[] = "lyric";
static const char KEY_NAME_NOTE_ROM[] = "rom";
static const char KEY_NAME_NOTE_START[] = "on";
static const char KEY_NAME_NOTE_DURATION[] = "dur";

VoiceLibMetadata VoiceLibMetadata::fromObject(const QJsonObject &obj) {
    VoiceLibMetadata meta;

    QJsonFormatter f;
    bool parsed = f.parse(obj, f.OT({
                                   {KEY_NAME_VOICE_NAME, f.S(&meta.name)},
                                   {KEY_NAME_VOICE_ID, f.S(&meta.id)},
                                   {KEY_NAME_VOICE_VERSION, f.S(&meta.version)},
                                   {KEY_NAME_VOICE_BUILT_BY, f.S(&meta.builtBy)},
                                   {KEY_NAME_VOICE_VOICE_BY, f.S(&meta.voiceBy)},
                               }));

    if (!parsed) {
        return VoiceLibMetadata();
    }

    return meta;
}

QJsonObject VoiceLibMetadata::toObject() const {
    return QJsonObject({{KEY_NAME_VOICE_NAME, name},
                        {KEY_NAME_VOICE_ID, id},
                        {KEY_NAME_VOICE_VERSION, version},
                        {KEY_NAME_VOICE_BUILT_BY, builtBy},
                        {KEY_NAME_VOICE_VOICE_BY, voiceBy}});
}

TPhoneme::TPhoneme() : ph(0), on(0), off(0) {
}

QJsonObject TPhoneme::toObject() const {
    return QJsonObject{
        {KEY_NAME_TPHONEME_PH, ph},
        {KEY_NAME_TPHONEME_ON, on},
        {KEY_NAME_TPHONEME_OFF, off},
    };
}

TNote::TNote() : pitch(60), on(0), off(0) {
}

QJsonObject TNote::toObject() const {
    return QJsonObject{
        {KEY_NAME_TNOTE_PITCH, pitch},
        {KEY_NAME_TNOTE_ON, on},
        {KEY_NAME_TNOTE_OFF, off},
    };
}

TChar::TChar() {
}

QJsonObject TChar::toObject() const {
    QJsonArray notesArr;
    for (const auto &note : qAsConst(notes)) {
        notesArr.append(note.toObject());
    }

    QJsonArray ipaArr;
    for (const auto &ph : qAsConst(ipa)) {
        ipaArr.append(ph.toObject());
    }

    return QJsonObject{
        {KEY_NAME_TCHAR_CH, ch},
        {KEY_NAME_TCHAR_ROM, rom},
        {KEY_NAME_TCHAR_NOTES, notesArr},
        {KEY_NAME_TCHAR_IPA, ipaArr},
    };
}

QJsonObject FNote::toObject() const {
    return QJsonObject({
        {KEY_NAME_NOTE_PITCH, pitch},
        {KEY_NAME_NOTE_LYRIC, lyric},
        {KEY_NAME_NOTE_ROM, rom},
        {KEY_NAME_NOTE_START, start},
        {KEY_NAME_NOTE_DURATION, duration},
    });
}

QJsonObject FUtt::toObject() const {
    QJsonArray notesArr;
    for (const auto &note : qAsConst(notes)) {
        notesArr.append(note.toObject());
    }
    return QJsonObject({
        {KEY_NAME_UTTERANCE_NAME, name},
        {KEY_NAME_UTTERANCE_SINGER, singerId},
        {KEY_NAME_UTTERANCE_ROM_SCHEME, romScheme},
        {KEY_NAME_UTTERANCE_NOTES, notesArr},
    });
}

QJsonObject SynthArgs::toObject() const {
    return QJsonObject({
        {"bpm0", bpm0},
        {"fUtt", utterance.toObject()},
        {"path", path},
    });
}
