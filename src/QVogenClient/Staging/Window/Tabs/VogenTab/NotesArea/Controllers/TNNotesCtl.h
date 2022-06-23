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

    void moveNotes(const QList<TWNote::Movement> &moves);
    void stretchNotes(const QList<TWNote::Stretch> &stretches);

public:
    void selectAll();
    void deselect();

    bool isMoving() const;
    bool isStretching() const;

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

    QPointF m_startPoint;

    QList<MovingData> m_movingData;
    QList<StretchingData> m_stretchingData;

protected:
    quint64 m_noteMaxId;

    QList<TNNoteGroup *> m_noteGroups; // Utterances
    TNNoteGroup *m_mainGroup;
    TNNoteGroup *m_currentGroup;

    QMap<quint64, TNRectNote *> m_noteMap;
    TNNoteList *m_timeBounds; // All

    TNNoteList *m_selection; // Index

    void switchGroup(TNNoteGroup *group);

    TNRectNote *createNote(quint64 id = 0);

    void adjustGeometry(TNRectNote *note);
    void adjustGroupGeometry(const TNNoteGroup *group);
    void adjustAllGeometry();
    void adjustCanvas();

    void selectOne(TNRectNote *p);
    void deselectOne(TNRectNote *p);

    void setGroupSelected(TNNoteGroup *group, bool selected);
    void setGroupEnabled(TNNoteGroup *group, bool enabled);

    bool eventFilter(QObject *obj, QEvent *event) override;

protected:
    int startTick() const;
    int totalLength() const;

private:
    void _q_beginChanged(int index, int val);
    void _q_endChanged(int index, int val);
};

#endif // TNNOTESCTL_H
