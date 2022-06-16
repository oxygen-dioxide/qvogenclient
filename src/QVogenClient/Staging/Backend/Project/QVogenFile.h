#ifndef QVOGENFILE_H
#define QVOGENFILE_H

#include <QList>
#include <QString>

#include "CompressedFile.h"

/*
 * Usage: util to parse or write Vogen package file(.ust).
 */

class QVogenFile : public CompressedFile {
public:
    QVogenFile();
    QVogenFile(const QString &filename);
    ~QVogenFile();

    struct Phoneme {
        QString name;
        int start;
        int end;
    };

    struct Note {
        int pitch;
        QString lyric;
        QString rom;
        int start;
        int duration;

        QList<Phoneme> cg;
    };

    struct Utterance {
        QString name;
        QString singer;
        QString romScheme;
        QList<Note> notes;

        QList<double> f0;
    };

public:
    QString projectName;

    double tempo;
    QPoint beat;
    int accomOffset;

    QList<Utterance> utterances;

protected:
    bool loadCore(bool *valid) override;
    bool saveCore() override;
    void resetCore() override;
};

#endif // QVOGENFILE_H
