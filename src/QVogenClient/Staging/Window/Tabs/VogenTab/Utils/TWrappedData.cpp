#include "TWrappedData.h"
#include "QJsonFormatter.h"

const char KEY_NAME_NOTE_ID[] = "id";
const char KEY_NAME_NOTE_START[] = "start";
const char KEY_NAME_NOTE_LENGTH[] = "length";
const char KEY_NAME_NOTE_NOTE_NUM[] = "noteNum";
const char KEY_NAME_NOTE_LYRIC[] = "lyric";
const char KEY_NAME_NOTE_GROUP_ID[] = "groupId";

TWProject::TWProject() {
}

TWProject::~TWProject() {
}

QJsonObject TWNote::NoteAll::toJson() const {
    QJsonObject obj({{KEY_NAME_NOTE_ID, QString::number(id)},
                     {KEY_NAME_NOTE_START, start},
                     {KEY_NAME_NOTE_LENGTH, length},
                     {KEY_NAME_NOTE_NOTE_NUM, noteNum},
                     {KEY_NAME_NOTE_LYRIC, lyric},
                     {KEY_NAME_NOTE_GROUP_ID, QString::number(gid)}});
    return obj;
}

TWNote::NoteAll TWNote::NoteAll::fromJson(const QJsonObject &obj) {
    NoteAll note;

    QJsonFormatter f;

    QString idStr, gidStr;

    // Parse Object
    bool parsed = f.parse(obj, f.OT({
                                   {KEY_NAME_NOTE_ID, f.S(&idStr)},
                                   {KEY_NAME_NOTE_START, f.I(&note.start)},
                                   {KEY_NAME_NOTE_LENGTH, f.I(&note.length)},
                                   {KEY_NAME_NOTE_NOTE_NUM, f.I(&note.noteNum)},
                                   {KEY_NAME_NOTE_LYRIC, f.S(&note.lyric)},
                                   {KEY_NAME_NOTE_GROUP_ID, f.S(&gidStr)},
                               }));

    if (!parsed) {
        NoteAll err;
        err.id = 0;
        return err;
    }

    note.id = idStr.toULongLong();
    note.gid = gidStr.toULongLong();

    return note;
}
