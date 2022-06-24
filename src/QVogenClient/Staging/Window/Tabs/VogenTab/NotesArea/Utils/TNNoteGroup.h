#ifndef TNNOTEGROUP_H
#define TNNOTEGROUP_H

#include "../Elements/TNRectNote.h"

#include "TNNoteListTmp.h"

//Q_DECLARE_ENTITY_LIST(TNNoteList, TNRectNote)

class TNNoteGroup : public TNNoteList {
    Q_OBJECT
public:
    explicit TNNoteGroup(QObject *parent = nullptr);
    ~TNNoteGroup();

public:
    quint64 id;

    QString name;
    QString singer;
    QString rom;

signals:
};

#endif // TNNOTEGROUP_H
