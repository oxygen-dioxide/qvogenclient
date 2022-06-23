#ifndef TNPITCHPOINT_H
#define TNPITCHPOINT_H

#include "TNRectPoint.h"

class TNPitchPoint : public TNRectPoint {
    Q_OBJECT
public:
    explicit TNPitchPoint(TNotesArea *area, QGraphicsItem *parent = nullptr);
    ~TNPitchPoint();

signals:
};

#endif // TNPITCHPOINT_H
