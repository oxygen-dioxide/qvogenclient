#ifndef TWRAPPEDDATA_H
#define TWRAPPEDDATA_H

#include <QList>
#include <QString>

#include <QJsonArray>
#include <QJsonObject>

class TWProject {
public:
    TWProject();
    ~TWProject();

    struct Phoneme {
        QString name;
        int start;
        int end;
    };

    struct Note {
        int noteNum;
        QString lyric;
        QString rom;
        int start;
        int length;
    };

    struct Utterance {
        QString name;
        QString singer;
        QString romScheme;
        QList<Note> notes;
    };

    QString projectName;

    double tempo;
    QPoint beat;
    int accomOffset;

    QList<Utterance> utterances;
};

namespace TWNote {

    struct Movement {
        quint64 id;
        int hMove;
        int vMove;
    };

    struct Stretch {
        quint64 id;
        int hStretch;
    };

    struct Lyric {
        quint64 id;
        QString lyric;
    };

    struct NoteAll {
        quint64 id;
        int start;
        int length;
        int noteNum;
        QString lyric;

        quint64 gid;

        QJsonObject toJson() const;
        static NoteAll fromJson(const QJsonObject &obj);
    };

} // namespace TWNote

#endif // TWRAPPEDDATA_H
