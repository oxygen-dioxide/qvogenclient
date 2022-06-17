#ifndef TNRECTPOINT_H
#define TNRECTPOINT_H

#include "TNRectObject.h"

class TNRectPoint : public TNRectObject {
    Q_OBJECT
public:
    explicit TNRectPoint(TNotesArea *area, QGraphicsItem *parent = nullptr);
    ~TNRectPoint();

private:
    void init();

protected:
signals:
};

#endif // TNRECTPOINT_H
