#ifndef COMMONTUNESTD_H
#define COMMONTUNESTD_H

#include <QString>

namespace CommonTuneStd {

    // Constants
    const char TONE_NAMES[] = "CCDDEFFGGAAB";
    const char TONE_NAME_SHARP[] = "#";
    const char LYRIC_R[] = "R";

    const int TIME_BASE = 480;
    const int TONE_NUMBER_BASE = 24;
    const int TONE_OCTAVE_MAX = 7;
    const int TONE_OCTAVE_MIN = 1;
    const int TONE_OCTAVE_STEPS = 12;

    const double DEFAULT_VALUE_TEMPO = 120.0;

    int ToneNameToToneNum(const QString &name);
    QString ToneNumToToneName(int num);
    QString ToneNumToToneName(int nameIndex, int octaveIndex);

    double TickToTime(int tick, double tempo);
    int TimeToTick(double time, double tempo);
    bool isRestLyric(const QString &oLyric);

    int StandardToneNum();

}; // namespace CommonTuneStd

#endif // COMMONTUNESTD_H
