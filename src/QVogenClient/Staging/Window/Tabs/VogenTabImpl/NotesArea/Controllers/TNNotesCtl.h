#ifndef TNNOTESCTL_H
#define TNNOTESCTL_H

#include "CommonNote.h"
#include "TNController.h"

#include "QNavigatableList.h"

class TNRectNote;

class TNNotesCtl : public TNController {
    Q_OBJECT
public:
    explicit TNNotesCtl(TNotesArea *parent = nullptr);
    ~TNNotesCtl();

public:
    void install() override;

    void setNotesFromCommon(const QList<CommonNote> &notes);

    const QList<TNRectNote *> &notes() const;

public:

protected:
    QList<TNRectNote *> m_notes;

    TNRectNote *createNote();
    void adjustGeometries();

    bool eventFilter(QObject *obj, QEvent *event) override;

protected:
    void insertNoteRef(TNRectNote *p);
    void removeNoteRef(TNRectNote *p);
    void updateNoteRefs(const QList<TNRectNote *> &notes);

    int startTick() const;
    int totalLength() const;

    struct NoteComparator_Start {
        bool operator()(TNRectNote *p1, const TNRectNote *p2) const;
    };
    struct NoteComparator_End {
        bool operator()(TNRectNote *p1, const TNRectNote *p2) const;
    };

    QNavigatableList<TNRectNote *, NoteComparator_Start> m_startRef;
    QNavigatableList<TNRectNote *, NoteComparator_End> m_endRef;

signals:
};

#endif // TNNOTESCTL_H
