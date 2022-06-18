#ifndef TNRECTNOTE_H
#define TNRECTNOTE_H

#include "TNRectObject.h"

class TNRectNote : public TNRectObject {
    Q_OBJECT
public:
    explicit TNRectNote(TNotesArea *area, QGraphicsItem *parent = nullptr);
    ~TNRectNote();

public:
    int start;
    int length;
    int tone;
    QString lyric;

    void layoutRequestEvent(QEvent *event) override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

signals:
};

#endif // TNRECTNOTE_H
