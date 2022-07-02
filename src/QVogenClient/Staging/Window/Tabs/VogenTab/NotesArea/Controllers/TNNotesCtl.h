#ifndef TNNOTESCTL_H
#define TNNOTESCTL_H

#include "TNController.h"

#include "../../Utils/TWrappedData.h"
#include "../Elements/TNRectScreen.h"
#include "../Utils/TNNoteGroup.h"

class TNNotesCtl : public TNController {
    Q_OBJECT
public:
    explicit TNNotesCtl(TNotesArea *parent = nullptr);
    ~TNNotesCtl();

public:
    void install() override;

    void addUtterances(const QList<TWProject::Utterance> &utters);
    QList<TWProject::Utterance> utterances() const;
    TWProject::Utterance currentValidUtterance() const;
    TWProject::Utterance validUtterance(quint64 gid) const;

    void moveNotes(const QList<TWNote::Movement> &moves);
    void stretchNotes(const QList<TWNote::Stretch> &stretches);
    void changeLyrics(const QList<TWNote::Lyric> &lyrics);
    void addNotes(const QList<TWNote::NoteAll> &notes, const TWNote::Group &group);
    void removeNotes(const QList<quint64> &ids);
    void changeGroup(const QList<quint64> &ids, const TWNote::Group &group);
    void changeSinger(quint64 gid, const QString &singerId, const QString &rom);

public:
    void selectAll();
    void deselect();

    bool isMoving() const;
    bool isStretching() const;
    bool isDrawing() const;
    bool isLyricsEditing() const;

    bool hasSelection() const;
    quint64 currentGroupId() const;
    QList<quint64> groupIdList() const;

    void setGroupCache(quint64 id, const QList<double> &pitches, const QString &waveFile);
    void removeGroupCache(quint64 id);
    void removeAllCache();
    bool hasCache(quint64 id) const;

    QList<QPair<qint64, QWaveInfo *>> audioData() const;

protected:
    struct MovingData {
        TNRectNote *note;
        QPointF originPos;
        int dx;
        int dy;
    };

    struct StretchingData {
        TNRectNote *note;
        QPointF originPos;
        QSizeF originSize;
        int dw;
    };

    struct DrawingData {
        TNRectNote *note;
        int dw;
    };

    QPointF m_startPoint;

    int m_movedNoteIndex;
    QList<MovingData> m_movingData;
    QList<StretchingData> m_stretchingData;
    QList<DrawingData> m_drawingData;

protected:
    quint64 m_maxNoteId;
    quint64 m_maxGroupId;

    QMap<quint64, TNNoteGroup *> m_noteGroups; // Utterances
    QSet<QString> m_groupNames;

    TNNoteGroup *m_mainGroup;
    TNNoteGroup *m_currentGroup;

    QMap<quint64, TNRectNote *> m_noteMap;
    TNNoteList *m_timeBounds; // All

    TNNoteList *m_selection; // Index

    bool m_editing;
    TNNoteList *m_cachedSelection;
    QList<QPair<TNRectNote *, QString>> m_cachedLyrics;

    TNRectScreen *m_waveScreen;
    void switchGroup(TNNoteGroup *group);
    TNNoteGroup *findGroup(quint64 gid) const;

    // Note Variations
    TNRectNote *createNote(quint64 id, int start, int len, int tone, const QString &lrc,
                           TNNoteGroup *g);
    void removeNote(TNRectNote *p);
    void changeNoteGroup(TNRectNote *p, TNNoteGroup *g);

    void setNotesMovable(bool movable);

    // Group Variations
    TNNoteGroup *createGroup(quint64 id, const QString &name, const QString &singer,
                             const QString &rom);
    void removeGroup(TNNoteGroup *g);

    void adjustGeometry(TNRectNote *note);
    void adjustGroupGeometry(const TNNoteGroup *group);

    // 牵一发而动全身
    void invalidGroup(TNNoteGroup *group);
    void updatePlayState();

public:
    void updateScreen();

    void adjustAllGeometry();
    void adjustAllGroupHintPos();
    void adjustCanvas();

protected:
    void selectOne(TNRectNote *p);
    void deselectOne(TNRectNote *p);

    void setGroupSelected(TNNoteGroup *group, bool selected);
    void setGroupEnabled(TNNoteGroup *group, bool enabled);

    // Helper
    QList<TNRectNote *> tryApplyLyrics(int len, TNNoteList *s);
    void addUtterancesCore(const QList<TWProject::Utterance> &utters,
                           QList<QPair<TNNoteGroup *, QList<TNRectNote *>>> *res = nullptr);

    TWProject::Utterance getUtterance(TNNoteGroup *group, bool ignore) const;

    bool eventFilter(QObject *obj, QEvent *event) override;

public:
    int startTick() const;
    int totalLength() const;
    double audioStartTime() const;
    double audioEndTime() const;

private:
    void _q_beginChanged(int index, int oldIndex, TNRectNote *p);
    void _q_endChanged(int index, int oldIndex, TNRectNote *p);
};

#endif // TNNOTESCTL_H
