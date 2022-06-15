#ifndef COMMONNOTE_H
#define COMMONNOTE_H

#include <QString>

#include "CommonTuneStd.h"

class CommonNote {
public:
    CommonNote(int start = 0,                                  //
               int length = CommonTuneStd::TIME_BASE,          //
               int noteNum = CommonTuneStd::StandardToneNum(), //
               const QString lyric = QString(),                //
               double tempo = CommonTuneStd::DEFAULT_VALUE_TEMPO);

    ~CommonNote();

public:
    int start;
    int length;
    int noteNum;

    QString lyric;
    double tempo;
};

#endif // COMMONNOTE_H
