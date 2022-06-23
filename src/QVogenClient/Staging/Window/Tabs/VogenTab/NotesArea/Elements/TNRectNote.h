#ifndef TNRECTNOTE_H
#define TNRECTNOTE_H

#include "TNRectSelectable.h"

class TNNoteGroup;

class TNRectNote : public TNRectSelectable {
    Q_OBJECT
public:
    explicit TNRectNote(TNotesArea *area, QGraphicsItem *parent = nullptr);
    ~TNRectNote();

public:
    int start;
    int length;
    int tone;
    QString lyric;

    TNNoteGroup *group;

    Behavior mousePressBehavior() const;

protected:
    bool m_movable;
    bool m_stretch;

    void setStretch(bool stretch);

    void layoutRequestEvent(QEvent *event) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

public:
    int type() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

signals:
};

#endif // TNRECTNOTE_H
