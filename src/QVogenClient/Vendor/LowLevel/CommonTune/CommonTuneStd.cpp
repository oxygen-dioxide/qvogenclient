#include "CommonTuneStd.h"

#define NUM2STR(str) QString::number(str)

int CommonTuneStd::ToneNameToToneNum(const QString &name) {
    int octave;

    if (name.length() < 2) {
        return TONE_NUMBER_BASE;
    }

    int index = QString(TONE_NAMES).indexOf(name.front());
    if (index < 0) {
        index = 0;
    }

    octave = name.rightRef(1).toInt();

    if (octave < TONE_OCTAVE_MIN) {
        octave = TONE_OCTAVE_MIN;
    } else if (octave > TONE_OCTAVE_MAX) {
        octave = TONE_OCTAVE_MAX;
    }

    return TONE_NUMBER_BASE + (octave - 1) * TONE_OCTAVE_STEPS + index +
           static_cast<int>(name[1] == TONE_NAME_SHARP);
}

QString CommonTuneStd::ToneNumToToneName(int num) {
    return ToneNumToToneName(num % TONE_OCTAVE_STEPS, num / TONE_OCTAVE_STEPS - 2);
}

QString CommonTuneStd::ToneNumToToneName(int nameIndex, int octaveIndex) {
    QString tone_names(TONE_NAMES);
    QString name = tone_names.at(nameIndex);
    if (nameIndex > 0 && tone_names.at(nameIndex) == tone_names.at(nameIndex - 1)) {
        name += TONE_NAME_SHARP;
    }
    name += NUM2STR(octaveIndex + 1);
    return name;
}

double CommonTuneStd::TickToTime(int tick, double tempo) {
    return (static_cast<unsigned long>(60000) * tick) / (tempo * TIME_BASE);
}

int CommonTuneStd::TimeToTick(double time, double tempo) {
    return static_cast<int>(time * tempo * TIME_BASE / 60000);
}

bool CommonTuneStd::isRestLyric(const QString &lyric) {
    QString lrc = lyric.trimmed();
    return (lrc.isEmpty() || !lrc.compare(LYRIC_R, Qt::CaseInsensitive));
}

int CommonTuneStd::StandardToneNum() {
    return (4 - TONE_OCTAVE_MIN) * TONE_OCTAVE_STEPS + TONE_NUMBER_BASE;
}
