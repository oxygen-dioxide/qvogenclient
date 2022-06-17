#ifndef TNRUBBER_H
#define TNRUBBER_H

#include <QGraphicsRectItem>

#include "QRectStyle.h"

class TNotesArea;

class TNRubber : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    explicit TNRubber(TNotesArea *area, QGraphicsItem *parent = nullptr);
    ~TNRubber();

private:
    void init();

public:
    QRectStyle rectStyle() const;
    void setRectStyle(const QRectStyle &rectStyle);

    QRect edgeRect() const;
    void setEdgeRect(const QRect &edgeRect);

    bool active() const;

    void start(bool continuous = false);
    void stop(QRectF *region = nullptr);

protected:
    TNotesArea *m_area;

    QRectStyle m_rectStyle;

    QPointF m_start;
    QPointF m_end;

    QRect m_edgeRect;

    int m_timerId;
    bool m_continuous;

    void setStartPos(const QPointF &pos);
    void setEndPos(const QPointF &pos);

    void updateSelect();

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

protected:

    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    void timerEvent(QTimerEvent *event) override;
};

#endif // TNRUBBER_H
