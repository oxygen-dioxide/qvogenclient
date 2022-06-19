#include "QVogenVoiceFile.h"

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

#include <quazip.h>

const char KEY_NAME_VOICE_NAME[] = "name";
const char KEY_NAME_VOICE_ID[] = "id";
const char KEY_NAME_VOICE_VERSION[] = "version";
const char KEY_NAME_VOICE_BUILT_BY[] = "builtBy";
const char KEY_NAME_VOICE_VOICE_BY[] = "voiceBy";

static const char Slash = '/';

QVogenVoiceFile::QVogenVoiceFile() {
    reset();
}

QVogenVoiceFile::QVogenVoiceFile(const QString &filename) {
    setFilename(filename);
}

QVogenVoiceFile::~QVogenVoiceFile() {
}

bool QVogenVoiceFile::install() {
    if (id.isEmpty()) {
        return false;
    }

    m_tempDir = qData->getStandardPath(DataManager::Voice) + Slash + id;
    if (Sys::isDirExist(m_tempDir)) {
        m_tempDir.clear();
        return false;
    }

    bool res = CompressedFile::loadCore();
    if (!res) {
        m_tempDir.clear();
        return false;
    }

    m_tempDir.clear();
    return true;
}

bool QVogenVoiceFile::loadCore(bool *valid) {
    bool alloc = false;
    TempDirGuard guard;

    if (m_tempDir.isEmpty()) {
        alloc = true;
        guard.create();
        m_tempDir = guard.path();
        if (m_tempDir.isEmpty()) {
            return false;
        }
    }

    // Extract
    bool res = CompressedFile::loadCore(valid);
    if (!res) {
        if (alloc) {
            m_tempDir.clear();
        }
        return false;
    }

    // Read chart.json
    QFile file(m_tempDir + "/meta.json");
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
        res = false;
    }

    if (alloc) {
        m_tempDir.clear();
    }

    return res;
}

bool QVogenVoiceFile::saveCore() {
    return false;
}

void QVogenVoiceFile::resetCore() {
    name.clear();
    id.clear();
    version.clear();
    builtBy.clear();
    voiceBy.clear();
    dir.clear();
}
