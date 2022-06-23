#include "TNRubber.h"

#include <QCursor>
#include <QGraphicsScene>
#include <QPainter>
#include <QScrollBar>

#include "../TNotesArea.h"
#include "../TNotesScroll.h"

TNRubber::TNRubber(TNotesArea *area, QGraphicsItem *parent) : TNRectObject(area, parent) {
    init();
}

TNRubber::~TNRubber() {
}

void TNRubber::init() {
    m_continuous = false;
    m_timerId = 0;
    m_edgeRect = QRect(QPoint(20, 20), QPoint(120, 20));

    m_rectStyle.setPen(QColor(QColor(0, 0, 0, 96)));
    m_rectStyle.setBrush(QColor(QColor(0, 0, 0, 48)));

    setVisible(false);
}

QRectStyle TNRubber::rectStyle() const {
    return m_rectStyle;
}

void TNRubber::setRectStyle(const QRectStyle &rectStyle) {
    m_rectStyle = rectStyle;
}

QRect TNRubber::edgeRect() const {
    return m_edgeRect;
}

void TNRubber::setEdgeRect(const QRect &edgeRect) {
    m_edgeRect = edgeRect;
}

bool TNRubber::active() const {
    return isVisible();
}

void TNRubber::start(bool continuous) {
    m_continuous = continuous;
    m_timerId = startTimer(10);

    setFocus();
    setVisible(true);
    this->grabMouse();

    setStartPos(m_area->mousePosition());
    updateSelect();
}

void TNRubber::stop(QRectF *region) {
    if (region) {
        *region = QRectF(this->pos(), this->rect().size());
    }

    setStartPos(QPointF(0, 0));
    setEndPos(QPointF(1, 1));

    this->ungrabMouse();
    setVisible(false);

    killTimer(m_timerId);
    m_timerId = 0;
    m_continuous = false;
}

void TNRubber::setStartPos(const QPointF &pos) {
    m_start = pos;
    setEndPos(pos + QPointF(1, 1));
}

void TNRubber::setEndPos(const QPointF &pos) {
    m_end = pos;

    int m_left = qMin(m_start.x(), m_end.x());
    int m_width = qAbs(m_start.x() - m_end.x());
    int m_top = qMin(m_start.y(), m_end.y());
    int m_height = qAbs(m_start.y() - m_end.y());

    if (m_continuous) {
        setPos(m_left, 0);
        setRect(0, 0, m_width, scene()->sceneRect().height());
    } else {
        setPos(m_left, m_top);
        setRect(0, 0, m_width, m_height);
    }
}

void TNRubber::updateSelect() {
    auto scroll = m_area->view();
    QPoint globalCursor = QCursor::pos();
    QPoint scrollCursor = scroll->viewport()->mapFromGlobal(globalCursor);
    QScrollBar *bar;

    setEndPos(m_area->mousePosition());

    int value, toValue;
    int offset;
    double ratio = 0.5;

    const QRect &frameSize = edgeRect();

    bar = scroll->horizontalScrollBar();
    value = bar->value();

    if (scrollCursor.x() > scroll->viewport()->width() - frameSize.right()) {
        offset = scrollCursor.x() - (scroll->viewport()->width() - frameSize.right());
        toValue = value + offset * ratio;
        bar->setValue(toValue);
    } else if (scrollCursor.x() < frameSize.left()) {
        offset = scrollCursor.x() - frameSize.left();
        toValue = value + offset * ratio;
        bar->setValue(toValue);
    }

    bar = scroll->verticalScrollBar();
    value = bar->value();

    if (scrollCursor.y() > scroll->viewport()->height() - frameSize.bottom()) {
        offset = scrollCursor.y() - (scroll->viewport()->height() - frameSize.bottom());
        toValue = value + offset * ratio;
        bar->setValue(toValue);
    } else if (scrollCursor.y() < frameSize.top()) {
        offset = scrollCursor.y() - frameSize.top();
        toValue = value + offset * ratio;
        bar->setValue(toValue);
    }
}

void TNRubber::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setPen(m_rectStyle.pen());
    painter->setBrush(m_rectStyle.brush());

    QRectF rect = this->rect();
    rect.adjust(1, 1, -1, -1);

    painter->drawRect(rect);
}

void TNRubber::focusInEvent(QFocusEvent *event) {
    QGraphicsRectItem::focusInEvent(event);
}

void TNRubber::focusOutEvent(QFocusEvent *event) {
    QGraphicsRectItem::focusOutEvent(event);
}

void TNRubber::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event)
    updateSelect();
}
