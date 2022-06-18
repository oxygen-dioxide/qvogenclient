#ifndef TNRECTSCREEN_H
#define TNRECTSCREEN_H

#include "TNRectObject.h"

class TNRectScreen : public TNRectObject {
public:
    explicit TNRectScreen(TNotesArea *area, QGraphicsItem *parent = nullptr);
    ~TNRectScreen();

public:
    QPainterPath shape() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
};

#endif // TNRECTSCREEN_H
