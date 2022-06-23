#ifndef TNRECTSCREEN_H
#define TNRECTSCREEN_H

#include "TNRectObject.h"

class TNRectScreen : public TNRectObject {
    Q_OBJECT
public:
    explicit TNRectScreen(TNotesArea *area, QGraphicsItem *parent = nullptr);
    ~TNRectScreen();

public:
    QPainterPath shape() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
};

#endif // TNRECTSCREEN_H
