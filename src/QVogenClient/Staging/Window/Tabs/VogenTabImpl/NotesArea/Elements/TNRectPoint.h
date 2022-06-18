#ifndef TNRECTPOINT_H
#define TNRECTPOINT_H

#include "TNRectSelectable.h"

class TNRectPoint : public TNRectSelectable {
    Q_OBJECT
public:
    explicit TNRectPoint(TNotesArea *area, QGraphicsItem *parent = nullptr);
    ~TNRectPoint();

signals:
};

#endif // TNRECTPOINT_H
