#include "TNRectNote.h"
#include "../TNotesArea.h"

#include <QDebug>
#include <QPainter>

TNRectNote::TNRectNote(TNotesArea *area, QGraphicsItem *parent) : TNRectObject(area, parent) {
}

TNRectNote::~TNRectNote() {
}

void TNRectNote::layoutRequestEvent(QEvent *event) {
    Q_UNUSED(event);

    QRectF geometry = m_area->convertValueToGeometry(start, tone, length);
    setPos(geometry.topLeft());
    setRect(0, 0, geometry.width(), geometry.height());

    update();
}

void TNRectNote::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
//    qDebug() << option << widget;

    QRectF rect = this->rect();
    bool selected = isSelected();

    double padding = 0.5;
    double innerPadding = 2;
    double border = rect.height() / 10;

    border = border > 6 ? 6 : border;

    QColor lineColor = 0x78AD35;
    QColor fillColor = 0x78AD35;

    QRectF originRect(padding, padding, rect.width() - 2 * padding, rect.height() - 2 * padding);
    QRectF entityRect(originRect);
    QRectF textRect(originRect);
    textRect.setX(entityRect.x() + 5 + m_area->currentWidth() / 128.0);

    // Draw Entity
    painter->setBrush(lineColor);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(entityRect, border * 1.5, border * 1.5);

    // Draw Line
    if (selected) {
        painter->setBrush(fillColor);
        QRectF innerRect(entityRect.left() + innerPadding, entityRect.top() + innerPadding,
                         entityRect.width() - 2 * innerPadding,
                         entityRect.height() - 2 * innerPadding);
        painter->drawRoundedRect(innerRect, border * 1.5, border * 1.5);
    }

    painter->setPen(Qt::black);
    painter->setBrush(Qt::NoBrush);

    QFont font = m_area->font();
    font.setPixelSize(font.pixelSize() * 3 / 4);
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignVCenter, lyric, &entityRect);
}
