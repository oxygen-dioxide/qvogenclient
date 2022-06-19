#include "QVogenVoiceInfo.h"

#include "DataManager.h"
#include "QJsonFormatter.h"
#include "SystemHelper.h"
#include "TempDirGuard.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

const char KEY_NAME_VOICE_NAME[] = "name";
const char KEY_NAME_VOICE_ID[] = "id";
const char KEY_NAME_VOICE_VERSION[] = "version";
const char KEY_NAME_VOICE_BUILT_BY[] = "builtBy";
const char KEY_NAME_VOICE_VOICE_BY[] = "voiceBy";

QVogenVoiceInfo::QVogenVoiceInfo() {
    reset();
}

QVogenVoiceInfo::QVogenVoiceInfo(const QString &dirname) {
    setDirname(dirname);
}

QVogenVoiceInfo::~QVogenVoiceInfo() {
}

bool QVogenVoiceInfo::loadCore(const QString &filename, bool *valid) {
    Q_UNUSED(valid);

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QByteArray bytes = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(bytes, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }

    QJsonFormatter f;

    QJsonObject objDoc = doc.object();

    // Parse Document
    bool parsed = f.parse(objDoc, f.OT({
                                      {KEY_NAME_VOICE_NAME, f.S(&name)},
                                      {KEY_NAME_VOICE_ID, f.S(&id)},
                                      {KEY_NAME_VOICE_VERSION, f.S(&version)},
                                      {KEY_NAME_VOICE_BUILT_BY, f.S(&builtBy)},
                                      {KEY_NAME_VOICE_VOICE_BY, f.S(&voiceBy)},
                                  }));

    if (!parsed) {
        return false;
    }

    return true;
}

bool QVogenVoiceInfo::saveCore(const QString &filename) {
    Q_UNUSED(filename);
    return false;
}

void QVogenVoiceInfo::resetCore() {
    name.clear();
    id.clear();
    version.clear();
    builtBy.clear();
    voiceBy.clear();
}

QString QVogenVoiceInfo::configName() const {
    return "meta.json";
}
