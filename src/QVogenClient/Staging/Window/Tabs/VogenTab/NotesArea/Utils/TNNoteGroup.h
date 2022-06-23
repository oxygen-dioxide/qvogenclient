#ifndef TNNOTEGROUP_H
#define TNNOTEGROUP_H

#include "TNEntityListT.h"

#include "../Elements/TNRectNote.h"

Q_DECLARE_ENTITY_LIST(TNNoteList, TNRectNote)

class TNNoteGroup : public TNNoteList {
    Q_OBJECT
public:
    explicit TNNoteGroup(QObject *parent = nullptr);
    ~TNNoteGroup();

public:
    QString name;
    QString singer;
    QString rom;

signals:
};

#endif // TNNOTEGROUP_H
