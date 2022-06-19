#ifndef TNNOTESCTL_H
#define TNNOTESCTL_H

#include "CommonNote.h"
#include "TNController.h"

#include "../Utils/TNEntityList.h"

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
    void selectAll();
    void deselect();

protected:
    QList<TNRectNote *> m_notes;

    TNEntityList *m_timeBounds; // Index
    TNEntityList *m_selection;  // Index

    TNRectNote *createNote();

    void adjustGeometry(TNRectNote *note);
    void adjustGeometries();
    void adjustCanvas();

    void selectOne(TNRectNote *p);
    void deselectOne(TNRectNote *p);

    bool eventFilter(QObject *obj, QEvent *event) override;

protected:
    int startTick() const;
    int totalLength() const;

private:
    void _q_beginChanged(int index, int val);
    void _q_endChanged(int index, int val);
};

#endif // TNNOTESCTL_H
