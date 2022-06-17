#ifndef QUTAUSTD_H
#define QUTAUSTD_H

/***
 *
 * UTAU Standard Variables And Methods (Qt Lib)
 * Copyright: Sinestr, 2019-2022
 *
 * Contents: Data Structures, Ust Sections, Standard Filenames, Render Utils
 * Scripts in UTAU folder do not rely on external functions other than QT.
 *
 ***/

#include <QDebug>
#include <QMetaType>
#include <QString>

#ifdef QUTAUSTD_BUILD_STATIC
#define QUTAUSTD_EXPORT
#else
#define QUTAUSTD_EXPORT Q_DECL_EXPORT
#endif

const char NODEF_STRING[] = "%NODEF%";
const int NODEF_INT = INT_MIN;
const double NODEF_DOUBLE = -1e+08;

namespace Utau {

    // Classes
    class Point;
    class Note;
    class Genon;

    // Structs
    struct PBStrings {
        QString PBS;
        QString PBW;
        QString PBY;
        QString PBM;
    };

    // Types
    enum PointType { sJoin, linearJoin, rJoin, jJoin };

    enum PrefixMap { Prefix, Suffix };

    enum NoteProperty {
        Intensity,
        Modulation,
        Velocity,
        Lyric,
        Flags,
        NoteNum,
        Length,
        PreUtterance,
        VoiceOverlap,
        StartPoint,
        Tempo,
        PitchBend,
        PitchType,
        PitchStart,
        Portamento,
        Vibrato,
        Envelope,
    };

    QString QUTAUSTD_EXPORT fromFrqName(const QString &filename);
    QString QUTAUSTD_EXPORT toFrqName(const QString &filename);
    QString QUTAUSTD_EXPORT mrqName();

    QList<Point> QUTAUSTD_EXPORT DefaultEnvelope();
    QList<double> QUTAUSTD_EXPORT DefaultVibrato();
    QList<Point> QUTAUSTD_EXPORT DefaultPortamento();

    QString QUTAUSTD_EXPORT PointToString(PointType type);
    PointType QUTAUSTD_EXPORT StringToPoint(const QString &name);

    QList<Point> QUTAUSTD_EXPORT StringToPortamento(const PBStrings &pbstr);
    PBStrings QUTAUSTD_EXPORT PortamentoToString(const QList<Point> &points);

    QList<Point> QUTAUSTD_EXPORT StringToEnvelope(const QString &str);
    QString QUTAUSTD_EXPORT EnvelopeToString(const QList<Point> &points);

    Genon QUTAUSTD_EXPORT StringToGenon(const QString &str);
    QString QUTAUSTD_EXPORT GenonToString(const Genon &genon);

    QList<double> QUTAUSTD_EXPORT StringsToDoubles(const QStringList &strs);
    QStringList QUTAUSTD_EXPORT DoublesToStrings(const QList<double> &nums);

    int QUTAUSTD_EXPORT ToneNameToToneNum(const QString &name);
    QString QUTAUSTD_EXPORT ToneNumToToneName(int num);
    QString QUTAUSTD_EXPORT ToneNumToToneName(int nameIndex, int octaveIndex);

    double QUTAUSTD_EXPORT TickToTime(int tick, double tempo);
    int QUTAUSTD_EXPORT TimeToTick(double time, double tempo);
    bool QUTAUSTD_EXPORT isRestLyric(const QString &oLyric);

    int QUTAUSTD_EXPORT StandardToneNum();

    // Constants
    const char TONE_NAMES[] = "CCDDEFFGGAAB";
    const char TONE_NAME_SHARP[] = "#";
    const char LYRIC_R[] = "R";

    const int TIME_BASE = 480;
    const int TONE_NUMBER_BASE = 24;
    const int TONE_OCTAVE_MAX = 7;
    const int TONE_OCTAVE_MIN = 1;
    const int TONE_OCTAVE_STEPS = 12;

    // Default
    const double DEFAULT_VALUE_INTENSITY = 100.0;
    const double DEFAULT_VALUE_MODULATION = 100.0; // Add
    const double DEFAULT_VALUE_PRE_UTTERANCE = 0.0;
    const double DEFAULT_VALUE_TEMPO = 120.0;
    const double DEFAULT_VALUE_VOICE_OVERLAP = 0.0;

    const double DEFAULT_VALUE_VELOCITY = 100.0;
    const double DEFAULT_VALUE_START_POINT = 0.0;
    const char DEFAULT_VALUE_FLAGS[] = "";

    // Values
    const char VALUE_MODE2_ON[] = "True";
    const char VALUE_TRACKS_SINGLE[] = "1";
    const char VALUE_PITCH_TYPE[] = "5";

    const char UST_VERSION_1_2[] = "1.2";

    // Escape
    const char DIR_VOICE_BASE[] = "%VOICE%";

} // namespace Utau

#endif // QUTAUSTD_H
