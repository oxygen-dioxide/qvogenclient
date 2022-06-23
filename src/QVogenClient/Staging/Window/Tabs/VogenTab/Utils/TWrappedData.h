#ifndef TWRAPPEDDATA_H
#define TWRAPPEDDATA_H

#include <QList>
#include <QString>

class TWrappedData {
public:
    TWrappedData();
    ~TWrappedData();

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

#endif // TWRAPPEDDATA_H
