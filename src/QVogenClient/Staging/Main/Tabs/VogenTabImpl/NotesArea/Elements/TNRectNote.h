#ifndef TNRECTNOTE_H
#define TNRECTNOTE_H

#include "TNRectObject.h"

class TNRectNote : public TNRectObject {
    Q_OBJECT
public:
    explicit TNRectNote(TNotesArea *area, QGraphicsItem *parent = nullptr);
    ~TNRectNote();

private:
    void init();

protected:
signals:
};

#endif // TNRECTNOTE_H
