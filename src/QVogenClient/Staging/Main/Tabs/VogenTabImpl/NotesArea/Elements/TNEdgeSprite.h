#ifndef TNEDGESPRITE_H
#define TNEDGESPRITE_H

#include <QGraphicsPixmapItem>

class TNotesArea;

class TNEdgeSprite : public QGraphicsPixmapItem {
public:
    explicit TNEdgeSprite(TNotesArea *area, QGraphicsItem *parent = nullptr);
    ~TNEdgeSprite();

public:
    TNotesArea *area() const;

    QPainterPath shape() const override;

protected:
    TNotesArea *m_area;
};

#endif // TNEDGESPRITE_H
