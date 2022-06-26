#ifndef TNNOTEGROUP_H
#define TNNOTEGROUP_H

#include "../Elements/TNGroupHint.h"

#include "TNNoteListTmp.h"

class TNNoteGroup : public TNNoteList {
    Q_OBJECT
public:
    explicit TNNoteGroup(TNotesArea *area, QObject *parent = nullptr);
    ~TNNoteGroup();

    friend class TNGroupHint;

public:
    void install();
    void uninstall();

public:
    quint64 id;

    QString name;
    QString singer;
    QString rom;

    TNGroupHint *hintItem() const;

    void adjustHintGeometry();
    void adjustHintPos();

protected:
    TNotesArea *m_area;

    TNGroupHint *m_hintItem;
    TNRectNote *m_firstNote;

private:
    void _q_inserted(int beginIndex, int endIndex, TNRectNote *p);
    void _q_removed(int beginIndex, int endIndex, TNRectNote *p);
    void _q_beginChanged(int index, int oldIndex, TNRectNote *p);

signals:
};

#endif // TNNOTEGROUP_H
