#ifndef TNNOTESCTL_H
#define TNNOTESCTL_H

#include "TNController.h"

#include "../../Utils/TWrappedData.h"
#include "../Utils/TNNoteGroup.h"

class TNNotesCtl : public TNController {
    Q_OBJECT
public:
    explicit TNNotesCtl(TNotesArea *parent = nullptr);
    ~TNNotesCtl();

public:
    void install() override;

    void setUtterances(const QList<TWProject::Utterance> &utters);
    QList<TWProject::Utterance> utterances() const;

    void moveNotes(const QList<TWNote::Movement> &moves);
    void stretchNotes(const QList<TWNote::Stretch> &stretches);
    void changeLyrics(const QList<TWNote::Lyric> &lyrics);
    void addNotes(const QList<TWNote::NoteAll> &notes);
    void removeNotes(const QList<quint64> &ids);

public:
    void selectAll();
    void deselect();

    bool isMoving() const;
    bool isStretching() const;
    bool isDrawing() const;
    bool isLyricsEditing() const;

    bool hasSelection() const;

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

    QList<MovingData> m_movingData;
    QList<StretchingData> m_stretchingData;
    QList<DrawingData> m_drawingData;

protected:
    quint64 m_maxNoteId;
    quint64 m_maxGroupId;

    QMap<quint64, TNNoteGroup *> m_noteGroups; // Utterances
    TNNoteGroup *m_mainGroup;
    TNNoteGroup *m_currentGroup;

    QMap<quint64, TNRectNote *> m_noteMap;
    TNNoteList *m_timeBounds; // All

    TNNoteList *m_selection; // Index

    bool m_editing;
    QList<QPair<TNRectNote *, QString>> m_cachedLyrics;

    void switchGroup(TNNoteGroup *group);
    TNNoteGroup *findGroup(quint64 gid) const;

    // Note Variations
    TNRectNote *createNote(quint64 id, int start, int len, int tone, const QString &lrc,
                           TNNoteGroup *g);
    void removeNote(TNRectNote *p);

    // Group Variations
    TNNoteGroup *createGroup(quint64 id);
    void removeGroup(TNNoteGroup *g);

    void adjustGeometry(TNRectNote *note);
    void adjustGroupGeometry(const TNNoteGroup *group);
    void adjustAllGeometry();
    void adjustCanvas();

    void selectOne(TNRectNote *p);
    void deselectOne(TNRectNote *p);

    void setGroupSelected(TNNoteGroup *group, bool selected);
    void setGroupEnabled(TNNoteGroup *group, bool enabled);

    QList<TNRectNote *> tryApplyLyrics(int len);

    bool eventFilter(QObject *obj, QEvent *event) override;

protected:
    int startTick() const;
    int totalLength() const;

private:
    void _q_beginChanged(int index, int val);
    void _q_endChanged(int index, int val);
};

#endif // TNNOTESCTL_H
