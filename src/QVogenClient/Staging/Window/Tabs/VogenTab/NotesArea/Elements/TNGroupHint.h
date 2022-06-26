#ifndef TNGROUPHINT_H
#define TNGROUPHINT_H

#include "TNRectObject.h"

class TNNoteGroup;

class TNGroupHint : public TNRectObject {
    Q_OBJECT
public:
    explicit TNGroupHint(TNNoteGroup *g, QGraphicsItem *parent = nullptr);
    ~TNGroupHint();

public:
    int type() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

protected:
    TNNoteGroup *g;
    QSize m_padding;

    QString hintText() const;

    void layoutRequestEvent(QEvent *event) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

signals:
};

#endif // TNGROUPHINT_H
