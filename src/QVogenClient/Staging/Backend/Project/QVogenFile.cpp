#include "QVogenFile.h"

#include "QJsonFormatter.h"
#include "TempDirGuard.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <quazip.h>

const char KEY_NAME_TIME_SIG[] = "timeSig0";
const char KEY_NAME_BPM[] = "bpm0";
const char KEY_NAME_ACCOM_OFFSET[] = "accomOffset";
const char KEY_NAME_UTTERANCES[] = "utts";

const char KEY_NAME_UTTERANCE_NAME[] = "name";
const char KEY_NAME_UTTERANCE_SINGER[] = "singerId";
const char KEY_NAME_UTTERANCE_ROM_SCHEME[] = "romScheme";
const char KEY_NAME_UTTERANCE_NOTES[] = "notes";

const char KEY_NAME_NOTE_PITCH[] = "pitch";
const char KEY_NAME_NOTE_LYRIC[] = "lyric";
const char KEY_NAME_NOTE_ROM[] = "rom";
const char KEY_NAME_NOTE_START[] = "on";
const char KEY_NAME_NOTE_DURATION[] = "dur";

QVogenFile::QVogenFile() {
    reset();
}

QVogenFile::QVogenFile(const QString &filename) {
    setFilename(filename);
}

QVogenFile::~QVogenFile() {
}

bool QVogenFile::loadCore(bool *valid) {
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
    QFile file(m_tempDir + "/chart.json");
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

    QJsonObject objDoc = doc.object();

    // Temp Vars
    QString timeSig;

    QList<QJsonObject> utterances;
    Utterance utterance;

    QList<QJsonObject> notes;
    Note note;

    QJsonFormatter f;

    // Parse Document
    bool parsed = f.parse(objDoc, f.OT({
                                      {KEY_NAME_TIME_SIG, f.S(&timeSig)},
                                      {KEY_NAME_BPM, f.D(&tempo)},
                                      {KEY_NAME_ACCOM_OFFSET, f.I(&accomOffset)},
                                      {KEY_NAME_UTTERANCES, f.OL(&utterances)},
                                  }));

    if (!parsed) {
        res = false;
        goto out;
    }

    // Parse Time Sig
    {
        bool failed = false;
        QStringList pair = timeSig.split('/');
        if (pair.size() != 2) {
            failed = true;
        }
        if (!failed) {
            beat = QPoint(pair.front().toInt(), pair.back().toInt());
            if (beat.x() == 0 || beat.y() == 0) {
                failed = true;
            }
        }
        if (failed) {
            beat = QPoint(4, 4);
        }
    }

    // Parse Utterance
    for (auto it = utterances.begin(); it != utterances.end(); ++it) {
        utterance = Utterance();
        notes.clear();
        parsed = f.parse(*it, f.OT({
                                  {KEY_NAME_UTTERANCE_NAME, f.S(&utterance.name)},
                                  {KEY_NAME_UTTERANCE_SINGER, f.S(&utterance.name)},
                                  {KEY_NAME_UTTERANCE_ROM_SCHEME, f.S(&utterance.romScheme)},
                                  {KEY_NAME_UTTERANCE_NOTES, f.OL(&notes)},
                              }));
        if (!parsed) {
            res = false;
            goto out;
        }

        // Parse Note
        for (auto it2 = notes.begin(); it2 != notes.end(); ++it2) {
            note = Note();
            parsed = f.parse(*it2, f.OT({
                                       {KEY_NAME_NOTE_PITCH, f.I(&note.pitch)},
                                       {KEY_NAME_NOTE_LYRIC, f.S(&note.lyric)},
                                       {KEY_NAME_NOTE_ROM, f.S(&note.rom)},
                                       {KEY_NAME_NOTE_START, f.I(&note.start)},
                                       {KEY_NAME_NOTE_DURATION, f.I(&note.duration)},
                                   }));
            if (!parsed) {
                res = false;
                goto out;
            }

            utterance.notes.append(note);
        }

        this->utterances.append(utterance);
    }

// Over
out:
    if (alloc) {
        m_tempDir.clear();
    }

    return res;
}

bool QVogenFile::saveCore() {
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

    // Read chart.json
    QFile file(m_tempDir + "/chart.json");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (alloc) {
            m_tempDir.clear();
        }
        return false;
    }

    QJsonArray utterances;
    for (auto it = this->utterances.begin(); it != this->utterances.end(); ++it) {
        const auto &utterance = *it;

        QJsonArray notes;
        for (auto it2 = utterance.notes.begin(); it2 != utterance.notes.end(); ++it2) {
            // Marshall Note
            const auto &note = *it2;
            auto obj = QJsonObject({
                {KEY_NAME_NOTE_PITCH, note.pitch},
                {KEY_NAME_NOTE_LYRIC, note.lyric},
                {KEY_NAME_NOTE_ROM, note.rom},
                {KEY_NAME_NOTE_START, note.start},
                {KEY_NAME_NOTE_DURATION, note.duration},
            });
            notes.append(obj);
        }

        // Marshall Utterance
        auto obj = QJsonObject({
            {KEY_NAME_UTTERANCE_NAME, utterance.name},
            {KEY_NAME_UTTERANCE_SINGER, utterance.name},
            {KEY_NAME_UTTERANCE_ROM_SCHEME, utterance.romScheme},
            {KEY_NAME_UTTERANCE_NOTES, notes},
        });
        utterances.append(obj);
    }

    // Marshall Time Sig
    QString timeSig = QString::asprintf("%d/%d", beat.x(), beat.y());

    // Marshall Document
    QJsonObject objDoc = QJsonObject({
        {KEY_NAME_TIME_SIG, timeSig},
        {KEY_NAME_BPM, tempo},
        {KEY_NAME_ACCOM_OFFSET, accomOffset},
        {KEY_NAME_UTTERANCES, utterances},
    });

    QJsonDocument doc;
    doc.setObject(objDoc);

    // write chart.json
    file.write(doc.toJson());
    file.close();

    // Compress
    bool res = CompressedFile::saveCore();

    // Over
    if (alloc) {
        m_tempDir.clear();
    }

    return res;
}

void QVogenFile::resetCore() {
    tempo = 0;
    projectName.clear();
}
