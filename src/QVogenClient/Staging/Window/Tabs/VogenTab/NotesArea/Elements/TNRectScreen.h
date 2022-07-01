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

    QPixmap pixmap() const;
    void setPixmap(const QPixmap &pixmap);

protected:
    QPixmap m_pixmap;
};

#endif // TNRECTSCREEN_H
